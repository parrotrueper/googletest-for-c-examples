/*lint ++flb*/
/**
 * \file utilDdefines.h
 * \brief Common defines for
 * - utilDevDebug <---- use this one
 * - utilFullDevDebug
 * - utilTinyDevDebug
 */
/* clang-format off */
#ifndef UTILDDEFINES_H__
#define UTILDDEFINES_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "sys/cdefs.h"

/** \brief Helper macros for stringifying values, used in static asserts */
#define DD_STRINGIFY(x) #x
#define DD_MACRO2STR(x) DD_STRINGIFY(x)

/* False positives from pclint, are triggered because it does not find calls
to these macros even though we use them throughout the project.*/
/*lint -esym{755, DD_STATIC_ASSERT, DD_PRINTF, DD_PRINT_ASSERT, DD_TRACE} */
/*lint -esym{755, DD_TRACE_ROUTE, DD_ENTRY, DD_EXIT, TEST_CONDITION_FN} */
/*lint -esym{755, DD_Error_Handler, Error_Handler} */
#if DD_TRACE_EN
    #if DD_UNIT_TESTING
        #include <stdio.h> /* forbidden by MISRA so only use for off target testing */
    #else
        #include "mPrintf.h"
    #endif /* DD_UNIT_TESTING */
#endif

#if DD_UNIT_TESTING
    #include "utilUnitTestWrapper.h"
#endif /* DD_UNIT_TESTING*/

#ifndef __FUNC__
    #define __FUNC__ __func__
#endif /* __FUNC__ */

#ifndef __FNAME__
    #ifdef __FILE_NAME__
        #define __FNAME__   __FILE_NAME__
    #else
        #define __FNAME__   __FILE__
    #endif /* __FILE_NAME__ */
#endif /* __FNAME__ */

/** \brief Compile time assert */
#ifdef __cplusplus
    #if DD_UNIT_TESTING
      #define DD_STATIC_ASSERT static_assert
    #endif /* DD_UNIT_TESTING */
#else /* C */
    #define DD_STATIC_ASSERT _Static_assert
#endif

/* deliberate use of function-like macro to prevent printf use in production
code or remove support for Error Handler */
/*lint -esym{9026,DD_PRINTF,DD_PRINT_ASSERT,DD_TRACE,DD_TRACE_ROUTE,DD_ENTRY} */
/*lint -esym{9026,DD_EXIT,TEST_CONDITION_FN,DD_Error_Handler,Error_Handler} */
#if DD_TRACE_EN == 0
    #define DD_PRINTF(...)           ((void)0)
    #define DD_PRINT_ASSERT(...)     ((void)0)
    #define DD_TRACE(...)            ((void)0)
    #define DD_TRACE_ROUTE(...)      ((void)0)
    #define DD_ENTRY()               ((void)0)
    #define DD_EXIT()                ((void)0)
#endif /* DD_TRACE_EN == 0 */


/* when we are unit testing we may want to handle asserts differently  */
#if DD_UNIT_TESTING
    #define TEST_CONDITION_FN( fname, module, line, condition)               \
    do{ util_UTWrapper_Assert( fname, module, line, condition); }while(false)
#else
    #define TEST_CONDITION_FN( fname, module, line, condition) ((void)0)
#endif /* DD_UNIT_TESTING */

#ifdef DD_REPLACE_ERROR_HANDLER
    #define Error_Handler() DD_Error_Handler(__FNAME__,  __FUNC__, __LINE__)
#else
    #define DD_Error_Handler() ((void)0)
#endif
/*----------------------------------------------------------------------------*/




#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* UTILDDEFINES_H__ */
/* clang-format on */
/*lint --flb*/
