/*lint ++flb*/
/**
 * \file utilTinyDevDebug.h
 * \brief Tiny Debugging helper file for asserts and trace.
 * \note do not call this directly, include utilDevDebug.h instead
 */
/* clang-format off */
#ifndef UTILTINYDEVDEBUG_H__
#define UTILTINYDEVDEBUG_H__
#ifdef TINY_ASSERTS_ONLY
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */
/*----------------------------------------------------------------------------*/
#include "utilDdefines.h"

/**
 * \brief Private module function that runs on an assert event, called by the
 * assert macros not intended to be called outside of this module
 * \param in_file - filename string
 * \param in_function - function name string
 * \param in_line_number - line number of offence
 */
void utilDevDebug_handleAssertTiny(const char* in_file, const char* in_function,
    int32_t in_line_number);


/**
 * \brief DD_ASSERT is a wrapper for an assert function that does not halt, it
 * works on the target by printing to the designated UART port, the offending
 * function name and line number of the failed condition. On the host it prints
 * to stdout
 *
 * Use example:
 *
 * DD_ASSERT( some_condition );
 */
/*lint -esym(9024, DD_ASSERT) deliberate use of stringification in macro */
/*lint -esym(9026, DD_ASSERT) deliberate use of function-like macro */
 #define DD_ASSERT(bCondition)                                                 \
    do {                                                                       \
        if ( bCondition ){}else{                                               \
            utilDevDebug_handleAssertTiny(__FNAME__,  __FUNC__, __LINE__);     \
            /* notify the test harness that an assert occurred */              \
            TEST_CONDITION_FN(__FNAME__, __FUNC__, __LINE__, #bCondition);     \
        }                                                                      \
    } while ( false )
/**
 * \brief Error handler macro for the project, it prints the file and line
 * number of the offending code and then halts the system. It is intended to be
 * used in the project as a replacement for the default error handler, and to be
 * called by the assert macros when an assert condition is not met.
 * To enable, define the macro DD_REPLACE_ERROR_HANDLER to replace the default
 * error handler with this one. Remove the default one in your project.
 */
#ifdef DD_REPLACE_ERROR_HANDLER
#define DD_Error_Handler(filename, function, line)                             \
    do {                                                                       \
        utilDevDebug_handleAssertTiny(filename,  function, line);                  \
        TEST_CONDITION_FN( filename, function, line, "ERROR_HANDLER");         \
    } while ( false )
#endif /* DD_REPLACE_ERROR_HANDLER */

/* on a small embedded system we may not have enough space to include stdio */
#if DD_TRACE_EN && DD_UNIT_TESTING
    #define DD_PRINTF(...)  do{(void)printf(__VA_ARGS__); fflush(stdout); fflush(stderr);}while(false)
    #define DD_PRINT_ASSERT(...) do{(void)printf(__VA_ARGS__);}while(false)

    #define DD_TRACE(...) DD_PRINTF(__VA_ARGS__)

    #define DD_ENTRY()  do{DD_TRACE("Entry %s()\n",__FUNC__);}while(false)
    #define DD_EXIT()   do{DD_TRACE("Exit %s()\n",__FUNC__);}while(false)

    #define DD_TRACE_ROUTE(...)                                                \
        do {                                                                   \
            DD_TRACE("+ %s() L:%d -> ", __FUNC__, __LINE__);                   \
            DD_TRACE( __VA_ARGS__);                                            \
        } while ( false )

#endif /* DD_TRACE_EN && DD_UNIT_TESTING */

/**
 * For tiny asserts we limit the size of the stored strings to save space
 * The message format is:
 * "BUG:" + filename + "," + function + "()," + line number + "\n\0"
 */
#define DD_ASSERT_MSG_LEN            ((uint8_t)(4))
#define DD_ASSERT_MAX_FILENAME_LEN   ((uint8_t)(12))
#define DD_ASSERT_MAX_FUNCTION_LEN   ((uint8_t)(30))
#define DD_ASSERT_MAX_LINENUM_LEN ((uint8_t)(13))

/* commas () newline null terminator */
#define DD_ASSERT_EXTRA_CHARS ((uint8_t)(1U + 3U + 2U))
#define DD_ASSERT_BUFFER_SIZE (DD_ASSERT_MSG_LEN + DD_ASSERT_MAX_FILENAME_LEN + \
                               DD_ASSERT_MAX_FUNCTION_LEN + \
                               DD_ASSERT_MAX_LINENUM_LEN + DD_ASSERT_EXTRA_CHARS )


#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* TINY_ASSERTS_ONLY */
#endif /* UTILTINYDEVDEBUG_H__ */
