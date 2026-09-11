#include "dateTime.h"

bool dateTime_isLeapYear(uint16_t year)
{
    bool is_leap = false;

    if ( (year % 400U) == 0U ) {
        is_leap = true;
    } else if ( (year % 100U) == 0U ) {
        is_leap = false;
    } else if ( (year % 4U) == 0U ) {
        is_leap = true;
    } else {
        is_leap = false;
    }
    return is_leap;
}

uint8_t dateTime_getDaysInMonth(uint16_t year, uint8_t month)
{
    uint8_t days = 0U;
    switch ( month ) {
        case FEBRUARY:
            days = FEB_DAYS;
            if ( dateTime_isLeapYear(year) ) {
                days = FEB_LEAP_DAYS;
            }
            break;
        case APRIL:     /* deliberate fall through */
        case JUNE:      /* deliberate fall through */
        case SEPTEMBER: /* deliberate fall through */
        case NOVEMBER:
            days = DAYS_A6SN;
            break;
        default:
            days = DAYS_MAX;
            break;
    }
    return days;
}

bool dateTime_isValidDate(uint16_t year, uint8_t month, uint8_t day)
{
    bool valid = ((month >= JANUARY) && (month <= DECEMBER));

    valid = (valid && (day >= DAYS_MIN) && (day <= DAYS_MAX));

    if ( valid ) {
        uint8_t max_days = dateTime_getDaysInMonth(year, month);
        valid            = (day <= max_days);
    }
    return valid;
}

bool dateTime_isValidTime(uint8_t hour, uint8_t minute, uint8_t second,
                          uint32_t microseconds)
{
    return (hour <= HOURS_MAX) && (minute <= MINUTES_MAX)
           && (second <= SECONDS_MAX) && (microseconds <= MICROSECONDS_MAX);
}

bool dateTime_isValidDateTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second,
                              uint32_t microseconds)
{
    bool valid = dateTime_isValidDate(year, month, day)
                 && dateTime_isValidTime(hour, minute, second, microseconds);
    return valid;
}
