/*lint ++flb*/
/**
 * \file utilFullDevDebug.h
 * \brief Debugging helper file for asserts and trace.
 * \note do not call this directly, include utilDevDebug.h instead
 */
/* clang-format off */
#ifndef UTILFULLDEVDEBUG_H__
#define UTILFULLDEVDEBUG_H__
#ifndef TINY_ASSERTS_ONLY
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
 * \param in_expression - asserted condition string
 */
void utilDevDebug_handleAssert(const char* in_file, const char* in_function,
    int32_t in_line_number, const char* in_expression);

/**
 * macro for the print function to use
 **/
#if DD_TRACE_EN && DD_UNIT_TESTING
    #define DD_PRINTF(...)  do{(void)printf(__VA_ARGS__); fflush(stdout); fflush(stderr);}while(false)
    #define DD_PRINT_ASSERT(...) do{(void)printf(__VA_ARGS__);}while(false)
#endif



/**
 * \brief DD_TRACE is a printf wrapper that works on the target on the
 * designated UART and on the host on stdout
 *
 * DD_TRACE_ROUTE is a handy way of printing where the code goes by printing
 * function name and line number and a message you can specify, use it like this:
 * DD_TRACE_ROUTE("Reached this point in the code\n");
 */
#if DD_TRACE_EN
    #define DD_TRACE(...) DD_PRINTF(__VA_ARGS__)

    #define DD_ENTRY()  do{DD_TRACE("Entry %s()\n",__FUNC__);}while(false)
    #define DD_EXIT()   do{DD_TRACE("Exit %s()\n",__FUNC__);}while(false)

    #define DD_TRACE_ROUTE(...)                                                \
        do {                                                                   \
            DD_TRACE("+ %s() L:%d ->", __FUNC__, __LINE__);                    \
            DD_TRACE( __VA_ARGS__);                                            \
        } while ( false )
#endif /* DD_TRACE_EN */

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
/*lint -e9023 deliberate use of stringification in macro */
/*lint -e9024 deliberate use of function-like macro */
/*lint -e9026 deliberate use of function-like macro */
  #define DD_ASSERT(bCondition)                                                \
      do {                                                                     \
          if ( bCondition ){}else{                                             \
            DD_PRINT_ASSERT(                                                   \
                "\nBUG! ASSERTION FAILED in: :\n%s %s(), L:%d, A:(%s)\n\n",    \
                    __FNAME__, __FUNC__, __LINE__, #bCondition);               \
                utilDevDebug_handleAssert(__FNAME__,  __FUNC__, __LINE__,      \
                                                    #bCondition);              \
                TEST_CONDITION_FN( __FNAME__, __FUNC__, __LINE__,              \
                    #bCondition);                                              \
          }                                                                    \
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
  #define DD_Error_Handler(filename, function, line)                           \
    do {                                                                       \
        DD_PRINT_ASSERT(                                                       \
            "\nBUG! ERROR HANDLER CALLED: :\n%s %s(), L:%d\n\n",               \
                filename, function, line);                                     \
        utilDevDebug_handleAssert(filename,  function, line, "ERROR_HANDLER"); \
        TEST_CONDITION_FN( filename, function, line, "ERROR_HANDLER");         \
    } while ( false )
#endif /* DD_REPLACE_ERROR_HANDLER */

#define DD_ASSERT_FILENMAE_SIZE   (256U)
#define DD_ASSERT_FUNCTION_SIZE   (64U)
#define DD_ASSERT_EXPRESSION_SIZE (64U)

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* not defined TINY_ASSERTS_ONLY */
#endif /* UTILFULLDEVDEBUG_H__ */
/* clang-format on */
/*lint --flb*/
