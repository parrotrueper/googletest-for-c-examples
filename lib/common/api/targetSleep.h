/**
 * \file targetSleep.h
 * \brief provide a sleep function based on platform and OS
 */
#ifndef TARGETSLEEP_H__
#define TARGETSLEEP_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */


#if DD_UNIT_TESTING
    #define target_sleepMs(in_milliseconds) ((void)0)
#else
    #include <stdint.h>
void target_sleepMs(uint32_t in_milliseconds);
#endif /* DD_UNIT_TESTING */

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* TARGETSLEEP_H__ */
