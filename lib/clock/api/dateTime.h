#ifndef DATETIME_H__
#define DATETIME_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#define JANUARY   ((uint8_t)(1))
#define FEBRUARY  ((uint8_t)(2))
#define MARCH     ((uint8_t)(3))
#define APRIL     ((uint8_t)(4))
#define MAY       ((uint8_t)(5))
#define JUNE      ((uint8_t)(6))
#define JULY      ((uint8_t)(7))
#define AUGUST    ((uint8_t)(8))
#define SEPTEMBER ((uint8_t)(9))
#define OCTOBER   ((uint8_t)(10))
#define NOVEMBER  ((uint8_t)(11))
#define DECEMBER  ((uint8_t)(12))

#define DAYS_MAX         ((uint8_t)(31))
#define DAYS_A6SN        ((uint8_t)(30)) /* April, June, September, November */
#define HOURS_MAX        ((uint8_t)(23))
#define MINUTES_MAX      ((uint8_t)(59))
#define SECONDS_MAX      ((uint8_t)(59))
#define MICROSECONDS_MAX ((uint32_t)(999999))

#define DAYS_MIN         ((uint8_t)(1))
#define HOURS_MIN        ((uint8_t)(0))
#define MINUTES_MIN      ((uint8_t)(0))
#define SECONDS_MIN      ((uint8_t)(0))
#define MICROSECONDS_MIN ((uint32_t)(0))

#define FEB_DAYS      ((uint8_t)(28))
#define FEB_LEAP_DAYS ((uint8_t)(29))

bool dateTime_isLeapYear(uint16_t year);
uint8_t dateTime_getDaysInMonth(uint16_t year, uint8_t month);
bool dateTime_isValidDate(uint16_t year, uint8_t month, uint8_t day);
bool dateTime_isValidTime(uint8_t hour, uint8_t minute, uint8_t second,
                          uint32_t microseconds);
bool dateTime_isValidDateTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second,
                              uint32_t microseconds);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DATETIME_H__ */
