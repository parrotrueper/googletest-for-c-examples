/**
 * \file clockTick.h
 * \brief Wrapper for 1us counter, allows us te get sensible timestamps
 * Requires a 1us tick timer counter, provided by timeDriver_getAppTick_uS()
 */
#ifndef CLOCKTICK_H__
#define CLOCKTICK_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <stdint.h>

#define CLK_TICK_SECOND_US (uint32_t)(1000000)
#define CLK_TICK_MINUTE_S  (uint32_t)(60)
#define CLK_TICK_HOUR_MIN  (uint32_t)(60)
#define CLK_TICK_MINUTE_US (uint32_t)(CLK_TICK_SECOND_US * CLK_TICK_MINUTE_S)

typedef struct {
    uint32_t microseconds;
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
} st_clock_tick_timestamp_t;

/**
 * \brief initialise this subsystem, once the counter is running
 */
void clockTick_initialise(void);
/**
 * \brief get the current time
 * \param out_timestamp_ptr - pointer to where the result should be placed.
 * \returns the timestamp structure with the current time
 */
void clockTick_getTimeStamp(st_clock_tick_timestamp_t* out_timestamp_ptr);
/**
 * \brief Service clock - call this in the main loop, to maintain the tiestamp
 */
void clockTick_updateTime(void);

/**
 * \brief convert a timestamp to a string
 * \param in_timestamp - the timestamp to convert
 * \param in_dest - pointer to the destination buffer
 * \param in_string_size  - the size of the destination buffer (recommended is 24)
 * \return the length of the result string or 0 on error
 * \note if the string does not fit in the destination buffer the result is
 * truncated
 **/
uint32_t clockTick_getTimeString(st_clock_tick_timestamp_t in_timestamp,
                                 char* in_dest, uint32_t in_string_size);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* CLOCKTICK_H__ */
