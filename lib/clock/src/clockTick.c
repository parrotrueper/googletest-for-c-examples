/**
 * \file tim_app_tick_us.c
 * \brief keep track of time so that we can create sensible timestamps
 * Configure a HW timer to count once every 1us
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "clockTick.h"
#include "mString.h"
#include "timeDriver.h"
#include "utilDevDebug.h"

/* pclint false positives that say that static symbols are not referenced
when they clearly are used in this file */
/*lint -esym(528, f_is_initialised, f_current_timestamp, f_prev_tick_count, f_tmp_str, f_tmp_length) */
/*lint -esym(528, clockTick_getDelta, clockTick_addStrNum, clockTick_timeToString) */

static bool f_is_initialised = false;
static st_clock_tick_timestamp_t f_current_timestamp;
static uint32_t f_prev_tick_count = 0U;

static char f_tmp_str[100];
static uint32_t f_tmp_length;

/**
 * \brief calculates the number of ticks that have elapsed between calls
 */
static uint32_t clockTick_getDelta(void)
{
    uint32_t delta;
    uint32_t new_time_snapshot = timeDriver_getAppTick_uS();

    if ( f_is_initialised == false ) {
        delta            = 0U;
        f_is_initialised = true;
    } else if ( new_time_snapshot >= f_prev_tick_count ) {
        delta = new_time_snapshot - f_prev_tick_count;
    } else { /* wrapped around */
        delta = ((uint32_t)(TIMDRV_MAX_US_COUNT)-f_prev_tick_count)
                + new_time_snapshot + 1U;
    }
    f_prev_tick_count = new_time_snapshot;
    return delta;
}
/**
 * \brief string converter helper adds number to string in the format "x:"
 * \param in_number - number to convert
 * \param in_dest - pointer to the destination buffer
 * \param in_string_size  - space in the destination buffer
 * \param out_index_ptr   - index in the buffer
 * \returns true if all went well
 */
static bool clockTick_addStrNum(uint32_t in_number, char* in_dest,
                                uint32_t in_string_size,
                                uint32_t* out_index_ptr)
{
    bool result = (out_index_ptr != NULL);
    if ( result ) {
        *out_index_ptr += m_utoa_s(in_number, &in_dest[*out_index_ptr],
                                   (in_string_size - *out_index_ptr));

        result         = (0U
                  == m_strcat_s(in_dest, (in_string_size - *out_index_ptr),
                                        ":"));
        *out_index_ptr = *out_index_ptr + 1U;
    }

    return result;
}
/**
 * \brief convert hours, minutes, seconds and microseconds into a string and place
 * it into our local temporary buffer
 */
static bool clockTick_timeToString(uint32_t hr, uint32_t min, uint32_t sec,
                                   uint32_t us)
{
    bool is_start_found  = false;
    bool is_error        = false;
    uint32_t numbers[3U] = {hr, min, sec};
    f_tmp_str[0U]        = '\0';
    f_tmp_length         = (uint32_t)(sizeof(f_tmp_str));
    uint32_t index       = 0U;

    for ( uint32_t num_index = 0U; num_index < 3U; num_index++ ) {
        is_start_found = (is_start_found || (numbers[num_index] != 0U));
        if ( !is_error && is_start_found ) {
            is_error = clockTick_addStrNum(numbers[num_index], f_tmp_str,
                                           f_tmp_length, &index);
        }
    }
    if ( !is_error ) {
        (void)m_utoa_s(us, &f_tmp_str[index], (f_tmp_length - index));
    }

    return is_error;
}

/*******************************************************************************
 *                  EXPOSED INTERFACES
 ******************************************************************************/
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
                                 char* in_dest, uint32_t in_string_size)
{
    bool is_error  = false;
    uint32_t index = 0;

    is_error = ((in_dest == NULL) || (in_string_size == 0U));

    if ( !is_error ) {
        is_error = clockTick_timeToString(
            in_timestamp.hours, in_timestamp.minutes, in_timestamp.seconds,
            in_timestamp.microseconds);
    }
    if ( !is_error ) {
        uint32_t length = m_strlen_s(f_tmp_str, f_tmp_length);
        for ( index = 0U; index < length; index++ ) {
            if ( index == (in_string_size - 1U) ) {
                break;
            }
            in_dest[index] = f_tmp_str[index];
        }
        in_dest[index] = '\0';
    }

    return (is_error ? 0U : index);
}

/**
 * \brief Service clock - call this in the main loop, to maintain the tiestamp
 */
void clockTick_updateTime(void)
{
    uint32_t us_to_add = clockTick_getDelta();

    f_current_timestamp.microseconds += us_to_add;
    while ( f_current_timestamp.microseconds > (CLK_TICK_SECOND_US - 1U) ) {
        if ( f_current_timestamp.seconds < (CLK_TICK_MINUTE_S - 1U) ) {
            f_current_timestamp.seconds++;
        } else {
            f_current_timestamp.seconds = 0U;
            if ( f_current_timestamp.minutes < (CLK_TICK_HOUR_MIN - 1U) ) {
                f_current_timestamp.minutes++;
            } else {
                f_current_timestamp.minutes = 0U;
                f_current_timestamp.hours++;
            }
        }
        f_current_timestamp.microseconds -= CLK_TICK_SECOND_US;
    }
}
/**
 * \brief get the current time
 * \param out_timestamp_ptr - pointer to where the result should be placed.
 * \returns the timestamp structure with the current time
 */
void clockTick_getTimeStamp(st_clock_tick_timestamp_t* out_timestamp_ptr)
{
    DD_ASSERT(out_timestamp_ptr != NULL); /* prevent null pointer access */
    clockTick_updateTime();

    /*lint --e{613} the assert protects the following assignments from nullptr*/
    out_timestamp_ptr->microseconds = f_current_timestamp.microseconds;
    out_timestamp_ptr->seconds = f_current_timestamp.seconds; /*lint --e{613} */
    out_timestamp_ptr->minutes = f_current_timestamp.minutes; /*lint --e{613} */
    out_timestamp_ptr->hours   = f_current_timestamp.hours;   /*lint --e{613} */
}

/**
 * \brief initialise this subsystem, once the counter is running
 */
void clockTick_initialise(void)
{
    f_is_initialised                 = false;
    f_prev_tick_count                = 0U;
    f_current_timestamp.microseconds = 0U;
    f_current_timestamp.hours        = 0U;
    f_current_timestamp.minutes      = 0U;
    f_current_timestamp.seconds      = 0U;
    clockTick_updateTime();
}
