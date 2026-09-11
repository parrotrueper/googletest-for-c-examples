#include <stddef.h>

#include "clockEvents.h"

#define CLK_EV_MILLIS_US (uint32_t)(1000)

#define CLK_EV_SECOND_U64_US (uint64_t)(1000000)
#define CLK_EV_MINUTE_U64_US (uint64_t)(CLK_EV_SECOND_U64_US * 60U)
#define CLK_EV_HOUR_U64_US   (uint64_t)(CLK_EV_MINUTE_U64_US * 60U)

/**
 * \brief helper function to add time to a timestamp
 * \param in_timestamp - timestamp to add time to
 * \param in_us_to_add - time in microseconds to add
 * \returns timestamp with time added on
 */
static st_clock_tick_timestamp_t clock_event_addTime(
    const st_clock_tick_timestamp_t in_timestamp, uint64_t in_us_to_add)
{
    st_clock_tick_timestamp_t result_timestamp;

    uint64_t us = (uint64_t)(in_timestamp.microseconds)
                  + (uint64_t)(in_us_to_add);

    result_timestamp.seconds = in_timestamp.seconds;
    result_timestamp.minutes = in_timestamp.minutes;
    result_timestamp.hours   = in_timestamp.hours;

    while ( us > (CLK_TICK_SECOND_US - 1U) ) {
        if ( result_timestamp.seconds < (CLK_TICK_MINUTE_S - 1U) ) {
            result_timestamp.seconds++;
        } else {
            result_timestamp.seconds = 0U;
            if ( result_timestamp.minutes < (CLK_TICK_HOUR_MIN - 1U) ) {
                result_timestamp.minutes++;
            } else {
                result_timestamp.minutes = 0U;
                result_timestamp.hours++;
            }
        }
        us -= CLK_TICK_SECOND_US;
    }
    result_timestamp.microseconds = (uint32_t)(us);

    return result_timestamp;
}

/**
 * \brief helper function compares start time to current time and checks if the
 * period has expired
 * \param in_current_timestamp - current time
 * \param in_start_timestamp - start time
 * \param in_period_us - time period in microseconds
 */
static bool clock_event_compareTime(
    st_clock_tick_timestamp_t in_current_timestamp,
    st_clock_tick_timestamp_t in_start_timestamp, uint64_t in_period_us)
{
    bool result  = false;
    bool is_done = false;

    st_clock_tick_timestamp_t stExpireTime = clock_event_addTime(
        in_start_timestamp, in_period_us);

    /* if the current time is more or equal than expire, then it has elapsed */
    uint32_t current[4] = {
        in_current_timestamp.hours, in_current_timestamp.minutes,
        in_current_timestamp.seconds, in_current_timestamp.microseconds};
    uint32_t expire[4] = {stExpireTime.hours, stExpireTime.minutes,
                          stExpireTime.seconds, stExpireTime.microseconds};
    for ( uint32_t ix = 0U; ix < 3U; ix++ ) {
        if ( current[ix] > expire[ix] ) {
            result  = true;
            is_done = true;
        } else {
            if ( current[ix] < expire[ix] ) {
                is_done = true;
            }
        }
        if ( is_done ) {
            break;
        }
    }
    if ( !is_done ) { /* seconds equal */
        if ( in_current_timestamp.microseconds >= stExpireTime.microseconds ) {
            result = true;
        }
    }

    return result;
}

/**
 * \brief helper function converts timestamp to microseconds
 * \param in_timestamp - the timestamp to convert
 * \result time in microseconds
 */
static uint64_t clock_event_tStampToUS(st_clock_tick_timestamp_t in_timestamp)
{
    uint64_t result;
    /* clang-format off */
    result =                   (uint64_t)(in_timestamp.microseconds)
        + (CLK_EV_SECOND_U64_US * (uint64_t)(in_timestamp.seconds     ))
        + (CLK_EV_MINUTE_U64_US * (uint64_t)(in_timestamp.minutes     ))
        + (CLK_EV_HOUR_U64_US   * (uint64_t)(in_timestamp.hours       ));
    /* clang-format on */
    return result;
}

/*******************************************************************************
 *                  EXPOSED INTERFACES
 ******************************************************************************/
/**
 * \brief Test if the specified time period has elapsed
 * \param in_start_timestamp - timestamp when the time period started
 * \param in_period_us - time in microseconds we want to elapse
 * \return true - if time has elapsed
 */
bool clock_event_hasUsTimeElapsed(st_clock_tick_timestamp_t in_start_timestamp,
                                  uint32_t in_period_us)
{
    bool has_elapsed;
    uint64_t us;
    st_clock_tick_timestamp_t current_timestamp;

    clockTick_getTimeStamp(&current_timestamp); /* get the current time */
    us = (uint64_t)(in_period_us);
    if ( us == 0U ) {
        has_elapsed = true;
    } else {
        has_elapsed = clock_event_compareTime(current_timestamp,
                                              in_start_timestamp, us);
    }
    return has_elapsed;
}

/**
 * \brief Test if the specified time period has elapsed
 * \param in_start_timestamp - timestamp when the time period started
 * \param in_period_ms - time in milliseconds we want to elapse
 * \return true - if time has elapsed
 */
bool clock_event_hasMsTimeElapsed(st_clock_tick_timestamp_t in_start_timestamp,
                                  uint32_t in_period_ms)
{
    bool has_elapsed;
    uint64_t us;
    st_clock_tick_timestamp_t current_timestamp;

    clockTick_getTimeStamp(&current_timestamp); /* get the current time */
    us = (uint64_t)(in_period_ms) * (uint64_t)(CLK_EV_MILLIS_US);
    if ( us == 0U ) {
        has_elapsed = true;
    } else {
        has_elapsed = clock_event_compareTime(current_timestamp,
                                              in_start_timestamp, us);
    }
    return has_elapsed;
}

/**
 * \brief Get the time that has elapsed between the provided time and the
 * current time
 * \param in_start_timestamp - timestamp when the time period started
 * \param out_timestamp_ptr - pointer to structure where the result should be saved
 */
void clock_event_stopWatch(st_clock_tick_timestamp_t in_start_timestamp,
                           st_clock_tick_timestamp_t* out_timestamp_ptr)
{
    st_clock_tick_timestamp_t current_timestamp;
    st_clock_tick_timestamp_t result_timestamp;
    st_clock_tick_timestamp_t difference_timestamp = {0U, 0U, 0U, 0U};
    uint32_t u32Add;
    uint64_t u64StartUs = 0U;
    uint64_t u64NowUs   = 0U;
    uint64_t u64Delta;
    if ( out_timestamp_ptr != NULL ) {
        clockTick_getTimeStamp(&current_timestamp);
        /* convert to microseconds to calculate the difference */
        u64StartUs = clock_event_tStampToUS(in_start_timestamp);
        u64NowUs   = clock_event_tStampToUS(current_timestamp);
        u64Delta   = u64NowUs - u64StartUs;
        if ( u64Delta > (uint64_t)(UINT32_MAX) ) {
            u32Add           = (uint32_t)(UINT32_MAX);
            result_timestamp = clock_event_addTime(difference_timestamp,
                                                   u32Add);
            difference_timestamp.hours        = result_timestamp.hours;
            difference_timestamp.minutes      = result_timestamp.minutes;
            difference_timestamp.seconds      = result_timestamp.seconds;
            difference_timestamp.microseconds = result_timestamp.microseconds;
            u64Delta -= (uint64_t)(UINT32_MAX);
        }
        u32Add           = (uint32_t)(u64Delta);
        result_timestamp = clock_event_addTime(difference_timestamp, u32Add);
        out_timestamp_ptr->hours        = result_timestamp.hours;
        out_timestamp_ptr->minutes      = result_timestamp.minutes;
        out_timestamp_ptr->seconds      = result_timestamp.seconds;
        out_timestamp_ptr->microseconds = result_timestamp.microseconds;
    }
}
