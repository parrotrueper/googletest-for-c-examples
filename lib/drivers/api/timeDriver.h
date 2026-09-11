#ifndef TIME_DRIVER_H__
#define TIME_DRIVER_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <stdint.h>

#define TIMDRV_MAX_US_COUNT (0xffffffffU)

void timeDriver_setupAppTick(void);

uint32_t timeDriver_getAppTick_uS(void);

void timeDriver_startAdcTimebase(void);

void timeDriver_stopAdcTimebase(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* TIME_DRIVER_H__ */
