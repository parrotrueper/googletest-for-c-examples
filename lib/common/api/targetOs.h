/**
 * \file targetOs.h
 * \brief decide what headers the application requires based on the operating
 * system we are building for
 */
#ifndef TARGETOS_H__
#define TARGETOS_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#ifdef OS_BARE_METAL
    #define target_sleepMs(in_miliseconds)
    #if DD_UNIT_TESTING
        #pragma message("Unit testing build")
    #endif /* DD_UNIT_TESTING */
#endif     /* OS_BARE_METAL */


#ifdef OS_THREADX
    #ifdef NEED_THREADX_MOCK /* for unit tests */
        #include "mockThreadX.h"
    #else
        #include "osThreadX.h"
    #endif /* NEED_THREADX_MOCK */
#endif     /* OS_THREADX */


#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* TARGETOS_H__ */
