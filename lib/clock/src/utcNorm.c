#include <stddef.h>

#include "dateTime.h"
#include "libAscii.h"
#include "utcNorm.h"

static int32_t utcNorm_shiftPositiveByDays(st_utc_time_t* utc_time,
                                           int32_t day_offset)
{
    while ( day_offset > 0 ) {
        uint8_t month_days = dateTime_getDaysInMonth(utc_time->year,
                                                     utc_time->month);
        utc_time->day++;
        if ( utc_time->day > month_days ) {
            utc_time->day = 1U;
            utc_time->month++;
            if ( utc_time->month > 12U ) {
                utc_time->month = 1U;
                utc_time->year++;
            }
        }
        day_offset--;
    }
    return day_offset;
}
static int32_t utcNorm_shiftNegativeByDays(st_utc_time_t* utc_time,
                                           int32_t day_offset)
{
    while ( day_offset < 0 ) {
        if ( utc_time->day > 1U ) {
            utc_time->day--;
        } else {
            if ( utc_time->month > 1U ) {
                utc_time->month--;
            } else {
                utc_time->month = 12U;
                utc_time->year--;
            }
            utc_time->day = dateTime_getDaysInMonth(utc_time->year,
                                                    utc_time->month);
        }
        day_offset++;
    }
    return day_offset;
}
static void utcNorm_shiftDateByDays(st_utc_time_t* utc_time, int32_t day_offset)
{
    day_offset = utcNorm_shiftPositiveByDays(utc_time, day_offset);

    (void)utcNorm_shiftNegativeByDays(utc_time, day_offset);
}
static void utcNorm_shiftNegativeMinutes(int32_t* total_minutes,
                                         int32_t* day_delta)
{
    while ( *total_minutes < 0 ) {
        *total_minutes += 1440;
        (*day_delta)--;
    }
}
static void utcNorm_shiftPositiveMinutes(int32_t* total_minutes,
                                         int32_t* day_delta)
{
    while ( *total_minutes >= 1440 ) {
        *total_minutes -= 1440;
        (*day_delta)++;
    }
}
/*******************************************************************************
 *                      Exposed Interfaces
 *****************************************************************************/
/* convert a time zone time to UTC +00:00 */
void utcNorm_toUtcZ(st_utc_time_t* utc_time)
{
    bool valid = (utc_time != NULL);
    if ( valid ) {
        if ( utc_time->zone.sign == KEY_UC_Z ) {
            valid = false; /* already UTC +00:00 */
        }
    }
    if ( valid ) {
        int32_t offset_minutes = ((int32_t)(utc_time->zone.hrs) * 60)
                                 + (int32_t)(utc_time->zone.mins);
        if ( utc_time->zone.sign == KEY_DASH ) {
            offset_minutes = -offset_minutes;
        }

        int32_t total_minutes = ((int32_t)(utc_time->hour) * 60)
                                + (int32_t)(utc_time->minute) - offset_minutes;
        int32_t day_delta = 0;
        utcNorm_shiftNegativeMinutes(&total_minutes, &day_delta);
        utcNorm_shiftPositiveMinutes(&total_minutes, &day_delta);

        int32_t hour     = total_minutes / 60;
        int32_t minute   = total_minutes % 60;
        utc_time->hour   = (uint8_t)hour;
        utc_time->minute = (uint8_t)minute;
        if ( day_delta != 0 ) {
            utcNorm_shiftDateByDays(utc_time, day_delta);
        }

        utc_time->zone.sign = (char_t)(KEY_UC_Z);
        utc_time->zone.hrs  = 0U;
        utc_time->zone.mins = 0U;
    }
}
