/**
 * \file clockEvents.h
 * \brief exposes clock event helpers
 **/
#ifndef CLOCKEVENTS_H__
#define CLOCKEVENTS_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */
#include <stdbool.h>

#include "clockTick.h"

/**
 * \brief Test if the specified time period has elapsed
 * \param in_start_timestamp - timestamp when the time period started
 * \param in_period_us - time in microseconds we want to elapse
 * \return true - if time has elapsed
 */
bool clock_event_hasUsTimeElapsed(st_clock_tick_timestamp_t in_start_timestamp,
                                  uint32_t in_period_us);

/**
 * \brief Test if the specified time period has elapsed
 * \param in_start_timestamp - timestamp when the time period started
 * \param in_period_ms - time in milliseconds we want to elapse
 * \return true - if time has elapsed
 */
bool clock_event_hasMsTimeElapsed(st_clock_tick_timestamp_t in_start_timestamp,
                                  uint32_t in_period_ms);

/**
 * \brief Get the time that has elapsed between the provided time and the
 * current time
 * \param in_start_timestamp - timestamp when the time period started
 * \param out_timestamp_ptr - pointer to structure where the result should be saved
 */
void clock_event_stopWatch(st_clock_tick_timestamp_t in_start_timestamp,
                           st_clock_tick_timestamp_t* out_timestamp_ptr);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* CLOCKEVENTS_H__ */
