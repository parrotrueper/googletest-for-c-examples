/*lint ++flb*/
/**
 * \file utilDevDebug.h
 * \brief Debugging helper file for asserts and trace.
 * \note Some macros require build definition DD_TRACE_EN=1.
 *
 * Implementation macros:
 *
 * DD_TRACE - alias to printf which can be enabled/disabled
 *          - requires build definition DD_TRACE_EN=1
 *
 * DD_PRINTF - alias to printf. Enabled if DD_TRACE_EN=1 or DD_UNIT_TESTING=1
 *
 * DD_TRACE_ROUTE - trace macro to indicate code path prints function name and
 *              line number
 *              - requires build definition DD_TRACE_EN=1
 *
 * DD_ASSERT(condition) - custom assert macro will call a safe state function
 *          and block. Enabled by default.
 *
 * DD_ENTRY() - trace macro to indicate function entry
 *
 * DD_EXIT() - trace macro to indicate function exit
 *
 * TEST_CONDITION_FN - unit test hook to handle asserts via the test framework
 *
 */
/* clang-format off */
#ifndef UTILDEVDEBUG_H__
#define UTILDEVDEBUG_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include "utilFullDevDebug.h"
#include "utilTinyDevDebug.h"

/**
 * \brief callback function to place the system in a safe state in an event
 * of an assert */
typedef void (*pf_safe_state_t)(void);

/**
 * \brief callback function to be used to consume any incoming messages
 * in the event of blowing an assert */
typedef void (*pf_consume_messages_t)(void);

/** \brief callback function to redirect output in the event of an assert
 * \param null terminated string of the filename
 * \param null terminated string of the function
 * \param null terminated string of the failed condition
 * \param line number of the failed condition
 */
typedef void (*pf_output_messages_t)(const char *filename, const char *function, const char *condition, int32_t line);

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
                              pf_safe_state_t in_safe_state);


/**
 * \brief DD_TRACE is a printf wrapper that works on the target on the
 * designated UART and on the host on stdout
 *
 * DD_TRACE_ROUTE is a handy way of printing where the code goes by printing
 * function name and line number
 *
 * Use example:
 *
 * DD_TRACE("Value is %d\n", some_value);
 */

 /**
  * \brief DD_TRACE_ROUTE is a code path trace macro that prints function name
  * and line number
  *
  * Use example:
  *
  * DD_TRACE_ROUTE();
  */

  /**
   * \brief DD_ENTRY and DD_EXIT are function entry and exit trace macros
   * that print function name
   *
   * Use example:
   *
   * void someFunction(void)
   * {
   *     DD_ENTRY();
   *     ...
   *     DD_EXIT();
   * }
   */


/**
 * \brief DD_ASSERT is a wrapper for an assert function that does not halt it
 * works on the target by printing to the designated UART port the offending
 * function name and line number of the failed condition. On the host it prints
 * to stdout
 *
 * Use example:
 *
 * DD_ASSERT( some_condition );
 */



#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* UTILDEVDEBUG_H__ */
/* clang-format on */
/*lint --flb*/
