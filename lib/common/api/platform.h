/**
 * \file platform.h
 * \brief decide what headers the application requires based on the target it
 * is being built for.
 * We also need this to tie 'int' to a type and comply with MISRA Directive 4.6
 * and to ensure the size of our types is what we are expecting.
 */
#ifndef PLATFORM_H__
#define PLATFORM_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#ifdef STM32F74xx
    #include "stm32f7xx_hal.h"
#endif /* STM32F74xx */

#if DD_UNIT_TESTING
    #include "mockPlatform.h"
#endif


#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* PLATFORM_H__ */
