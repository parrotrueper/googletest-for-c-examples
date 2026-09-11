/**
 * \file utilDevDebug.c
 * \brief redirect debug printf to the port of choice
 **/

#include "utilDevDebug.h"
#include <stddef.h>

#include "mString.h"
#include "targetSleep.h"


/* TODO: investigate false positive from pclint warning 528 refers to static
symbols not referenced */

/*lint -esym(528,f_assert_msg) */
/*lint -esym(528,f_msg_len) */
/*lint -esym(528,f_safe_state_handler) */
/*lint -esym(528,f_consumer_handler) */
/*lint -esym(528,f_publisher_handler) */
/*lint -esym(528,utilDevDebug_assertState) */
/*lint -esym(528,f_assert_filename) */
/*lint -esym(528,f_assert_function) */
/*lint -esym(528,f_assert_expression) */
/*lint -esym(528,f_assert_line_number) */
/*lint -esym(528,utilDevDebug_appendMsg) */

/*lint -esym(957, utilDevDebug_handleAssert) prototypes in Tiny and Full incs */


/* if we are unit testing we don't want to block on an assert */
#if DD_UNIT_TESTING
    #define ASSERT_STATE_ACTIVE     break
    #define ASSERT_STATE_RUN        have_entered_already = false
    #define DD_ASSERT_TRIGGER_COUNT (0U)
#else
    #define ASSERT_STATE_ACTIVE
    #define ASSERT_STATE_RUN
    #define DD_ASSERT_TRIGGER_COUNT (200U)
#endif

#ifdef TINY_ASSERTS_ONLY
static char f_assert_msg[DD_ASSERT_BUFFER_SIZE] = "BUG:";
static uint8_t f_msg_len                        = 0U;
#else
static char f_assert_filename[DD_ASSERT_FILENMAE_SIZE];
static char f_assert_function[DD_ASSERT_FUNCTION_SIZE];
static char f_assert_expression[DD_ASSERT_EXPRESSION_SIZE];
static int32_t f_assert_line_number;

#endif /* TINY_ASSERTS_ONLY */

/* call back functions for assert events */
/* place the system in a safe state  */
static pf_safe_state_t f_safe_state_handler = NULL;
/* consume any incoming messages */
static pf_consume_messages_t f_consumer_handler = NULL;
/* redirect output */
static pf_output_messages_t f_publisher_handler = NULL;

/*******************************************************************************
 *                      PRIVATE FUNCTIONS
 ******************************************************************************/
/**
 * \brief Private helper, blocking function on an assert event
 */
static void utilDevDebug_assertState(void)
{
    uint32_t sleep_count = 0;

    /* deliberately blocking as we are in a messed up state */
    for ( ;; ) {
        /* consume all incoming traffic */
        if ( f_consumer_handler != NULL ) {
            f_consumer_handler();
        }
        target_sleepMs(10U); /* sleep for 10ms */
        sleep_count++;

        if ( sleep_count > DD_ASSERT_TRIGGER_COUNT ) {
            /* send the ASSERT out on all ports */
            if ( f_publisher_handler != NULL ) {
#ifdef TINY_ASSERTS_ONLY
                f_publisher_handler(&f_assert_msg[0], '\0', '\0', f_msg_len);
#else
                f_publisher_handler(f_assert_filename, f_assert_function,
                                    f_assert_expression, f_assert_line_number);
#endif /* TINY_ASSERTS_ONLY */
            }
            sleep_count = 0;
        }
        ASSERT_STATE_ACTIVE;
    }
}
#ifdef TINY_ASSERTS_ONLY
/**
 * \brief append sting to assert message buffer
 * \param i_target - target buffer to append to
 * \param i_src - source string to append
 * \param i_len - maximum chars to append
 * \returns length of appended string
 * caller is responsible for ensuring no overflow occurs and no null pointers
 */
static uint8_t utilDevDebug_appendMsg(char* i_target, char* i_src,
                                      uint8_t i_len)
{
    uint8_t ix = 0;
    for ( ix = 0U; ix < i_len; ix++ ) {
        if ( i_src[ix] == '\0' ) {
            break;
        }
        i_target[ix] = i_src[ix];
    }
    return ix;
}
#endif /* TINY_ASSERTS_ONLY */
/*******************************************************************************
 *                      EXPOSED INTERFACES
 ******************************************************************************/
#ifdef TINY_ASSERTS_ONLY
/**
 * \brief Private module function that runs on an assert event, called by the
 * assert macros not intended to be called outside of this module
 * \param in_file - filename string
 * \param in_function - function name string
 * \param in_line_number - line number of offence
 * \param in_expression - asserted condition string
 */
void utilDevDebug_handleAssertTiny(const char* in_file, const char* in_function,
                                   int32_t in_line_number)
{
    static bool have_entered_already = false;
    uint8_t ix                       = 4U; /* start after "BUG " */

    if ( !have_entered_already ) {
        have_entered_already = true;
        /* save the filename */
        ix += utilDevDebug_appendMsg(&f_assert_msg[ix], (char*)in_file,
                                     DD_ASSERT_MAX_FILENAME_LEN);
        f_assert_msg[ix] = ','; /* add comma */
        ix++;
        /* save the function() */
        ix += utilDevDebug_appendMsg(&f_assert_msg[ix], (char*)in_function,
                                     DD_ASSERT_MAX_FUNCTION_LEN);
        /* add (), */
        ix += utilDevDebug_appendMsg(&f_assert_msg[ix], "(),", 3U);
        /* convert the line number to ascii */
        char lineStr[DD_ASSERT_MAX_LINENUM_LEN + 1U];
        lineStr[DD_ASSERT_MAX_LINENUM_LEN] = '\0';

        (void)m_itoa_s(in_line_number, lineStr, DD_ASSERT_MAX_LINENUM_LEN);
        ix += utilDevDebug_appendMsg(&f_assert_msg[ix], lineStr,
                                     DD_ASSERT_MAX_LINENUM_LEN);
        ix += utilDevDebug_appendMsg(&f_assert_msg[ix], "\n", 1);
        f_assert_msg[ix] = '\0';
        f_msg_len        = ix + 1U;

        /* Place the system in a safe state */
        if ( f_safe_state_handler != NULL ) {
            f_safe_state_handler();
        }
        utilDevDebug_assertState(); /* we do not return from this function */
        ASSERT_STATE_RUN;
    }
}

#else  /* TINY_ASSERTS_ONLY not defined */
/**
 * \brief Private module function that runs on an assert event, called by the
 * assert macros not intended to be called outside of this module
 * \param in_file - filename string
 * \param in_function - function name string
 * \param in_line_number - line number of offence
 * \param in_expression - asserted condition string
 */

void utilDevDebug_handleAssert(const char* in_file, const char* in_function,
                               int32_t in_line_number,
                               const char* in_expression)
{
    static bool have_entered_already = false;
    uint32_t length;

    if ( !have_entered_already ) {
        have_entered_already = true;

        /* save the filename */
        f_assert_filename[0U] = '\0';
        length                = (uint32_t)(sizeof(f_assert_filename));
        (void)m_strcat_s(f_assert_filename, length, in_file);
        /* save the function() */
        f_assert_function[0U] = '\0';
        length                = (uint32_t)(sizeof(f_assert_function));
        (void)m_strcat_s(f_assert_function, length, in_function);
        (void)m_strcat_s(f_assert_function, length, "()");
        /* save the line number */
        f_assert_line_number = in_line_number;
        /* save the (expression) */
        length                  = (uint32_t)(sizeof(f_assert_expression));
        f_assert_expression[0U] = '(';
        f_assert_expression[1U] = '\0';
        (void)m_strcat_s(f_assert_expression, length, in_expression);
        (void)m_strcat_s(f_assert_expression, length, ")");

        /* Place the system in a safe state */
        if ( f_safe_state_handler != NULL ) {
            f_safe_state_handler();
        }
        /* we do not return from this function */
        utilDevDebug_assertState();
        ASSERT_STATE_RUN;
    }
}
#endif /* TINY_ASSERTS_ONLY */

/**
 * \brief Initialise the debug system that handles assert events, call this
 * early on at boot up.
 * \param in_consumer - callback function to consume any incoming messages.
 *                       NULL if not applicable
 * \param in_publisher - callback function that sends the assert message to
 *                       a desired port. NULL if not applicable
 * \param in_safe_state - callback function that should be called on an
 *                       assert event which places the system in a safe state
 */
void utilDevDebug_initialise(pf_consume_messages_t in_consumer,
                             pf_output_messages_t in_publisher,
                             pf_safe_state_t in_safe_state)
{
    f_consumer_handler   = in_consumer;
    f_publisher_handler  = in_publisher;
    f_safe_state_handler = in_safe_state;
}


/*lint --flb*/
