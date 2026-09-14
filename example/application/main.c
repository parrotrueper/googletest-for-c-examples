/**
 * \file Main.c
 * \brief Implementation of the Example API
 */
#include <signal.h> /* SIGINT and friends */
#include <stdbool.h>
#include <stdint.h>
#include <string.h> /* strcmp */
#include <time.h>   /* nanosleep, timespec */

#include "autoVersion.h" /* git versions of this module and our dependencies */
#include "calcCrc32.h"
#include "counter.h"
#include "evaluate.h"
#include "globaltypes.h" /* target types such as int */
#include "helloWorld.h"
#include "main.h"         /* Our API */
#include "utilDevDebug.h" /* Debug and diagnostics helper */

static bool f_debug_enabled = false;
static const st_auto_version_t* version_info;

/****************************************************************/
/* Private Debugging Macros                                     */
/****************************************************************/
/** \def Enable/Disable trace debug in this module */
#define DD_MAIN_EN (0)
#if DD_MAIN_EN
    #if DD_COLOURIT_TRACE
        #define DD_MAIN_COLOUR DD_COLOUR_CYN
    #else
        #define DD_MAIN_COLOUR
    #endif

    #define DD_MAIN_TRACE(...)                                                 \
        do {                                                                   \
            if ( f_debug_enabled ) {                                           \
                DD_COL_TRACE(DD_MAIN_COLOUR, __VA_ARGS__);                     \
            }                                                                  \
        } while ( 0 )
#else
    #define DD_MAIN_TRACE(...) ((void)0)
#endif /* DD_MAIN_EN */

/****************************************************************/
/* Private Macros                                               */
/****************************************************************/
/** Eventlog name for this API */
#define API_EVENTLOG_NAME "Example API"
/** List of names for the threads used in this API */
#define MAIN_THREAD_RXIPC_NAME "Rx IPC"
#define MAIN_THREAD_TXIPC_NAME "Tx IPC"

/****************************************************************/
/* Private Typedefs                                             */
/****************************************************************/

/****************************************************************/
/* Private Variables */
/****************************************************************/
/** true if this module has been initialised*/
static bool f_main_initialised = false;
/** signal handler structure */
static struct sigaction f_sig_handler;
/** accessed by the signal handler see C11 draft standard n1570: 5.1.2.3 */
static volatile sig_atomic_t f_keep_running = 1;
/** true if we need to power down this API */
static bool f_shutdown_requested = false;

/****************************************************************/
/* Private Functions */
/****************************************************************/
/**
 * \brief Private function for the signal handler. Wired to SIGINT.
 * \param in_signal_number - received signal unused
 */
static void main_signalHandler(int_t in_signal_number)
{
    /** ATTENTION! only call async signal safe functions here see: [man 7
   * signal-safety]. */
    (void)(in_signal_number);
    f_keep_running = 0;
}

/**
 * \brief Private function to initialise this module. Call this first.
 * \return true if there are no errors
 */
static bool main_setup(void)
{
    uint8_t value[] = {0x5A, 0x5A, 0x5A, 0x5A};
    bool result     = true;
    int_t signal_result;

    DD_MAIN_TRACE("Example API Starting\n");
    DD_MAIN_TRACE("Entry %s()\n", __FUNC__);

    /* initialise our local variables */
    f_main_initialised   = false;
    f_shutdown_requested = false;
#ifdef DD_UNIT_TESTING
    f_keep_running = 0;
#else
    f_keep_running = 1;
#endif /* DD_UNIT_TESTING */

    counter_initialise();
    DD_MAIN_TRACE("Counter %s initialised\n",
                  (counter_isInitialised() ? "is" : "is NOT"));
    DD_MAIN_TRACE("Platform size of Float = %d\n",
                  evaluate_platformSizeOfFloat());
    counter_increment();
    DD_MAIN_TRACE("Platform size of Double = %d\n",
                  evaluate_platformSizeOfDouble());
    counter_increment();
    DD_MAIN_TRACE("Platform size of Long Double = %d\n",
                  evaluate_platformSizeOfLongDouble());
    counter_increment();
    DD_MAIN_TRACE("Pi 32-bit = %f\n", evaluate_getPi32bit());
    counter_increment();
    DD_MAIN_TRACE("Pi 64-bit = %f\n", evaluate_getPi64bit());
    counter_increment();
    DD_MAIN_TRACE("20/10 = %d\n", evaluate_divide(20, 10));
    counter_increment();
    DD_MAIN_TRACE("Value at address %p=%s\n", &value[0],
                  evaluate_showAddress(&value[0]));
    DD_MAIN_TRACE("Function calls=%d\n", counter_get());
    DD_MAIN_TRACE("CRC-32=%d\n", calcCrc32_Calc(&value[0], sizeof(value)));
    (void)value;
    DD_MAIN_TRACE("2023 %s leap year\n",
                  (util_DateTime_IsLeapYear(2023)) ? "is" : "is not");


    version_info = autoVersion_getInfo();

    /* git version of this module */
    DD_MAIN_TRACE("Version       %s\n", version_info->pVersion);
    DD_MAIN_TRACE("Commit Hash   %s\n", version_info->pCommitHash);
    DD_MAIN_TRACE("Branch        %s\n", version_info->pBranch);
    DD_MAIN_TRACE("Local Changes %d\n", version_info->pChanges);

    /* setup the signal handler */
    f_sig_handler.sa_handler = &main_signalHandler;
    signal_result            = sigaction(SIGINT, &f_sig_handler,
                                         NULL); /* only for SIGINT */
    if ( 0 != signal_result ) {
        DD_MAIN_TRACE("%s(), L:%d, [%d]\n", __FUNC__, __LINE__, signal_result);
        result = false;
    }

    DD_ASSERT(result);
    if ( result ) {
        f_main_initialised = true;
    }
    DD_MAIN_TRACE("Exit %s[%d]\n\n", __FUNC__, result);
    return result;
}

/**
 * \brief Private function to clean up to exit this API. Called by the main loop
 * and triggered by the f_shutdown_requested flag being set. Sends the shutdown
 * request to each of the running threads of this API.
 */
static void main_teardown(void)
{
    DD_MAIN_TRACE("Entry %s()\n", __FUNC__);

    f_main_initialised = false;

    DD_MAIN_TRACE(
        "%s()",
        __FUNC__); /* note: there is no function eventlog_close() available */

    /* tell any running threads or state machines that they need to terminate */
    DD_MAIN_TRACE("%s() System Shutting down...", __FUNC__);

    DD_MAIN_TRACE("Exit %s()\n", __FUNC__);
}
/**
 * \brief Private function used to request this API to start shutting down.
 * Flags the main loop to clean up and exit.
 */
static void main_shutdownRequest(void)
{
    f_shutdown_requested = true;
}

/**
 * \brief Private function to create the worker threads for this API.
 * \returns true if all went well
 */
static bool main_createThreads(void)
{
    bool result = false;

    DD_ASSERT(f_main_initialised);

    DD_MAIN_TRACE("Entry %s()\n", __FUNC__);

    /* we only go ahead with thread creation if we initialised without errors */
    if ( f_main_initialised ) {
        DD_MAIN_TRACE("Add code to create threads ...\n");
        DD_MAIN_TRACE("Add code that checks all threads are ready ...\n");
        result = true;
    } else {
        main_shutdownRequest();
    }

    DD_MAIN_TRACE("Exit %s()\n", __FUNC__);
    return result;
}

/**
 * \brief Private function that waits for all threads of this API to terminate.
 */
static void main_waitTermination(void)
{

    DD_MAIN_TRACE("Entry %s()\n", __FUNC__);

    DD_MAIN_TRACE("Add code that waits for the threads to actually stop...\n");
    DD_MAIN_TRACE("Add code that join successfully created threads...\n");

    DD_MAIN_TRACE("Example API Exiting\n");
    DD_MAIN_TRACE("Exit %s()\n", __FUNC__);
}

/**
 * \brief Parse the input parameters to the main function
 * \param argc - argument counter
 * \param argv - string argument parameter array
 * \return true if the code coverage parameter has been set
 */
static bool main_parseParameters(int_t argc, char** argv)
{
    bool code_coverage_enable = false;
    uint8_t arg_counter;
    uint8_t arg_count = argc;

    f_debug_enabled = false;
    /* check if this is running for code coverage -c or -C */
    for ( arg_counter = 0; arg_counter < (uint8_t)(arg_count); arg_counter++ ) {
        if ( argv[arg_counter] != NULL ) {
            /* code coverage check */
            if ( (0 == strcmp(argv[arg_counter], "-c"))
                 || (0 == strcmp(argv[arg_counter], "-C")) ) {
                code_coverage_enable = true;
                DD_TRACE(" Coverage run detected\n");
            } else
                /* debug enabled check */
                if ( (0 == strcmp(argv[arg_counter], "-d"))
                     || (0 == strcmp(argv[arg_counter], "-D")) ) {
                    f_debug_enabled = true;
                    DD_TRACE(" Debug enabled\n");
                }
        }
    }

    return code_coverage_enable;
}

/****************************************************************/
/* Exposed Interfaces */
/****************************************************************/

/**
 * \brief Public function and main entry to this API.
 * \param argc - argument counter
 * \param argv - string argument parameter array
 * \return 0 exited without errors, -1 exited with errors
 * \note This function is disabled for unit testing
 **/
#ifdef DD_UNIT_TESTING
int_t test_main(int_t argc, char** argv)
#else
int_t main(int_t argc, char** argv)
#endif /* DD_UNIT_TESTING */
{
    bool result;
    bool code_coverage_enable = false;
    int_t signal_result       = 0;
    uint8_t display_screen[15];

    struct timespec main_poll_sleep;
    const int32_t poll_time = (int32_t)(500000000); /* 500ms */

    DD_MAIN_TRACE("Entry: %s\n", __FUNC__);

    /* check if this is running for code coverage -c or -C */
    code_coverage_enable = main_parseParameters(argc, argv);

    /* initialise this API */
    result = main_setup();

    /* create worker threads */
    if ( result ) {
        result = main_createThreads();
    }

    /* Hello World! */
    result = helloWorld_main(&display_screen[0], sizeof(display_screen));
    (void)printf("\n%s\n", (char*)(&display_screen));

    /* sit here and wait for a request to shutdown or a signal = SIGINT */
    if ( result ) {
        main_poll_sleep.tv_sec  = 0;
        main_poll_sleep.tv_nsec = poll_time;
        do {
            (void)nanosleep(&main_poll_sleep, NULL);
            if ( code_coverage_enable ) {
                DD_TRACE("Exiting early for code coverage run\n");
                break;
            }
        } while ( (f_keep_running != 0) && !f_shutdown_requested );
    }

    if ( f_keep_running == 0 ) {
        DD_TRACE("\nSig int received.. shutting down\n");
    }

    /* if we are here we need to shutdown */
    main_teardown();

    main_waitTermination();

    if ( false == result ) {
        signal_result = -1;
    }

    return signal_result;
}
