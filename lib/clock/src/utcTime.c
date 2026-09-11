#include <stddef.h>

#include "dateTime.h"
#include "libAscii.h"
#include "mPrintf.h"
#include "mStrToNum.h"
#include "strToUtc.h"
#include "utcNorm.h"
#include "utcTime.h"
#include "utilDevDebug.h"

static st_utc_zone_t f_zone_Lut[] = {
    {KEY_UC_Z, 0, 0},   {KEY_DASH, 12, 0}, {KEY_DASH, 11, 0},
    {KEY_DASH, 10, 0},  {KEY_DASH, 9, 30}, {KEY_DASH, 9, 0},
    {KEY_DASH, 8, 0},   {KEY_DASH, 7, 0},  {KEY_DASH, 6, 0},
    {KEY_DASH, 5, 0},   {KEY_DASH, 4, 0},  {KEY_DASH, 3, 30},
    {KEY_DASH, 3, 0},   {KEY_DASH, 2, 0},  {KEY_DASH, 1, 0},
    {KEY_PLUS, 0, 0},   {KEY_PLUS, 1, 0},  {KEY_PLUS, 2, 0},
    {KEY_PLUS, 3, 0},   {KEY_PLUS, 3, 30}, {KEY_PLUS, 4, 0},
    {KEY_PLUS, 4, 30},  {KEY_PLUS, 5, 0},  {KEY_PLUS, 5, 30},
    {KEY_PLUS, 5, 45},  {KEY_PLUS, 6, 0},  {KEY_PLUS, 6, 30},
    {KEY_PLUS, 7, 0},   {KEY_PLUS, 8, 0},  {KEY_PLUS, 8, 45},
    {KEY_PLUS, 9, 0},   {KEY_PLUS, 9, 30}, {KEY_PLUS, 10, 0},
    {KEY_PLUS, 10, 30}, {KEY_PLUS, 11, 0}, {KEY_PLUS, 12, 0},
    {KEY_PLUS, 12, 45}, {KEY_PLUS, 13, 0}, {KEY_PLUS, 14, 0}};

/*******************************************************************************
 *                                 Private Functions
 ******************************************************************************/
/** \brief check if time zone is KEY_UC_Z */
static bool utcTime_isZoneZ(const st_utc_zone_t* zone)
{
    bool is_Z = false;
    if ( zone->sign == KEY_UC_Z ) {
        is_Z = true;
    } else {
        if ( (zone->sign == KEY_PLUS) && (zone->hrs == 0U)
             && (zone->mins == 0U) ) {
            is_Z = true;
        }
    }
    return is_Z;
}
/** \brief get zone sign */
static uint8_t utcTime_getZoneSign(const st_utc_zone_t zone)
{
    uint8_t sign = zone.sign;
    if ( zone.sign == KEY_UC_Z ) {
        sign = KEY_PLUS;
    }
    return sign;
}

/** \brief st_utc_time_t to ASCII up to SECONDS when the offset is '+/-hh:mm' */
static bool utcTime_toSecBuffer(const st_utc_time_t* utc_time, uint8_t* buffer,
                                uint16_t length)
{
    uint8_t sign = utcTime_getZoneSign(utc_time->zone);

    /* YYYY-MM-DDThh:mm:ss+00:00 */
    uint32_t len = m_snprintf_9(
        (char*)(buffer), length, "%04u-%02u-%02uT%02u:%02u:%02u%c%02u:%02u",
        utc_time->year, utc_time->month, utc_time->day, utc_time->hour,
        utc_time->minute, utc_time->second, sign, utc_time->zone.hrs,
        utc_time->zone.mins);
    return (len == UTC_SECONDS_FORMAT_LEN);
}
/** \brief st_utc_time_t to ASCII up to SECONDS when the offset is KEY_UC_Z */
static bool utcTime_toSecBufferZ(const st_utc_time_t* utc_time, uint8_t* buffer,
                                 uint16_t length)
{
    /* YYYY-MM-DDThh:mm:ssZ */
    uint32_t len = m_snprintf_6(
        (char*)(buffer), length, "%04d-%02d-%02dT%02d:%02d:%02dZ",
        utc_time->year, utc_time->month, utc_time->day, utc_time->hour,
        utc_time->minute, utc_time->second);
    return (len == UTC_SECONDS_FORMAT_LEN_Z);
}
/** \brief st_utc_time_t to ASCII up to MS when the offset is '+/-hh:mm' */
static bool utcTime_toMsBuffer(const st_utc_time_t* utc_time, uint8_t* buffer,
                               uint16_t length)
{
    uint8_t sign = utcTime_getZoneSign(utc_time->zone);

    /* YYYY-MM-DDThh:mm:ss.sss+00:00 */
    uint32_t milliseconds = utc_time->microseconds / 1000U;
    uint32_t len          = m_snprintf_10(
        (char*)(buffer), length,
        "%04u-%02u-%02uT%02u:%02u:%02u.%03u%c%02u:%02u", utc_time->year,
        utc_time->month, utc_time->day, utc_time->hour, utc_time->minute,
        utc_time->second, milliseconds, sign, utc_time->zone.hrs,
        utc_time->zone.mins);
    return (len == UTC_MS_FORMAT_LEN);
}
/** \brief st_utc_time_t to ASCII up to MS when the offset is KEY_UC_Z */
static bool utcTime_toMsBufferZ(const st_utc_time_t* utc_time, uint8_t* buffer,
                                uint16_t length)
{
    /* YYYY-MM-DDThh:mm:ss.sssZ */
    uint32_t milliseconds = utc_time->microseconds / 1000U;
    uint32_t len          = m_snprintf_7(
        (char*)(buffer), length, "%04u-%02u-%02uT%02u:%02u:%02u.%03uZ",
        utc_time->year, utc_time->month, utc_time->day, utc_time->hour,
        utc_time->minute, utc_time->second, milliseconds);
    return (len == UTC_MS_FORMAT_LEN_Z);
}
/** \brief st_utc_time_t to ASCII up to US when the offset is '+/-hh:mm' */
static bool utcTime_toUsBuffer(const st_utc_time_t* utc_time, uint8_t* buffer,
                               uint16_t length)
{
    uint8_t sign = utcTime_getZoneSign(utc_time->zone);

    /* YYYY-MM-DDThh:mm:ss.ssssss+00:00 */
    uint32_t len = m_snprintf_10(
        (char*)(buffer), length,
        "%04u-%02u-%02uT%02u:%02u:%02u.%06u%c%02u:%02u", utc_time->year,
        utc_time->month, utc_time->day, utc_time->hour, utc_time->minute,
        utc_time->second, utc_time->microseconds, sign, utc_time->zone.hrs,
        utc_time->zone.mins);
    return (len == UTC_US_FORMAT_LEN);
}
/** \brief st_utc_time_t to ASCII up to US when the offset is KEY_UC_Z */
static bool utcTime_toUsBufferZ(const st_utc_time_t* utc_time, uint8_t* buffer,
                                uint16_t length)
{
    /* YYYY-MM-DDThh:mm:ss.ssssssZ */
    uint32_t len = m_snprintf_7(
        (char*)(buffer), length, "%04u-%02u-%02uT%02u:%02u:%02u.%06uZ",
        utc_time->year, utc_time->month, utc_time->day, utc_time->hour,
        utc_time->minute, utc_time->second, utc_time->microseconds);
    return (len == UTC_US_FORMAT_LEN_Z);
}

static bool utcTime_formatToBuffer(bool is_zone_z,
                                   const st_utc_time_t* utc_time,
                                   e_utc_format_t format, uint8_t* buffer,
                                   uint16_t length)
{
    bool valid = false;
    /* clang-format off */
    switch ( format ) { /* clang format off to keep function under 40 lines */
        case eUtcSeconds:
                valid = utcTime_toSecBuffer(utc_time, buffer, length);    break;
        case eUtcSecondsZ:
            if ( is_zone_z ) {
                valid = utcTime_toSecBufferZ(utc_time, buffer, length);
            } else {
                valid = utcTime_toSecBuffer(utc_time, buffer, length);
            }
            break;
        case eUtcMilliseconds:
                valid = utcTime_toMsBuffer(utc_time, buffer, length);     break;
        case eUtcMillisecondsZ:
            if ( is_zone_z ) {
                valid = utcTime_toMsBufferZ(utc_time, buffer, length);
            } else {
                valid = utcTime_toMsBuffer(utc_time, buffer, length);
            }
            break;
        case eUtcMicroseconds:
               valid = utcTime_toUsBuffer(utc_time, buffer, length);      break;
        case eUtcMicrosecondsZ:
            if ( is_zone_z ) {
                valid = utcTime_toUsBufferZ(utc_time, buffer, length);
            } else {
                valid = utcTime_toUsBuffer(utc_time, buffer, length);
            }
            break;
        default: valid = false;  break; /* LCOV_EXCL_LINE */
    }
    /* clang-format on */
    return valid;
}
static bool utcTime_toBuffCheckParams(const st_utc_time_t* utc_time,
                                      e_utc_format_t format, uint8_t* buffer,
                                      uint16_t length)
{
    bool valid = false;
    /*#lizard forgives this switch */
    if ( (utc_time != NULL) && (buffer != NULL) ) {
        switch ( format ) {
            case eUtcSeconds:
                if ( length > UTC_SECONDS_FORMAT_LEN ) {
                    valid = true;
                }
                break;
            case eUtcSecondsZ:
                if ( length > UTC_SECONDS_FORMAT_LEN_Z ) {
                    valid = true;
                }
                break;
            case eUtcMilliseconds:
                if ( length > UTC_MS_FORMAT_LEN ) {
                    valid = true;
                }
                break;
            case eUtcMillisecondsZ:
                if ( length > UTC_MS_FORMAT_LEN_Z ) {
                    valid = true;
                }
                break;
            case eUtcMicroseconds:
                if ( length > UTC_US_FORMAT_LEN ) {
                    valid = true;
                }
                break;
            case eUtcMicrosecondsZ:
                if ( length > UTC_US_FORMAT_LEN_Z ) {
                    valid = true;
                }
                break;
            default:
                break;
        }
    }
    return valid;
}

/*******************************************************************************
 *                      Exposed Interfaces
 *****************************************************************************/
/**
 * Vaild string formats:
 * ix:01234567890123456789012345678901
 * -  YYYY-MM-DDThh:mm:ssZ                       stop=NA sign=19 length=20
 * -  YYYY-MM-DDThh:mm:ss.sssZ                   stop=19 sign=23 length=24
 * -  YYYY-MM-DDThh:mm:ss+00:00                  stop=NA sign=19 length=25
 * -  YYYY-MM-DDThh:mm:ss.ssssssZ                stop=19 sign=26 length=27
 * -  YYYY-MM-DDThh:mm:ss.sss+00:00              stop=19 sign=23 length=29
 * -  YYYY-MM-DDThh:mm:ss.ssssss+00:00           stop=19 sign=26 length=32
 */
bool utcTime_fromString(const uint8_t* buffer, uint16_t length,
                        st_utc_time_t* utc_time)
{
    return strToUtc_convert(buffer, length, utc_time);
}

bool utcTime_isGreater(const st_utc_time_t* t1, const st_utc_time_t* t2)
{
    bool is_greater = false;

    if ( (t1 != NULL) && (t2 != NULL) ) {
        st_utc_time_t ti_n = *t1;
        st_utc_time_t t2_n = *t2;
        utcNorm_toUtcZ(&ti_n);
        utcNorm_toUtcZ(&t2_n);

        const int64_t lhs[] = {(int64_t)ti_n.year,        (int64_t)ti_n.month,
                               (int64_t)ti_n.day,         (int64_t)ti_n.hour,
                               (int64_t)ti_n.minute,      (int64_t)ti_n.second,
                               (int64_t)ti_n.microseconds};
        const int64_t rhs[] = {(int64_t)t2_n.year,        (int64_t)t2_n.month,
                               (int64_t)t2_n.day,         (int64_t)t2_n.hour,
                               (int64_t)t2_n.minute,      (int64_t)t2_n.second,
                               (int64_t)t2_n.microseconds};

        for ( size_t ix = 0U; ix < (sizeof(lhs) / sizeof(lhs[0])); ++ix ) {
            if ( lhs[ix] != rhs[ix] ) {
                is_greater = (lhs[ix] > rhs[ix]);
                break;
            }
        }
    }

    return is_greater;
}
void utcTime_copy(const st_utc_time_t* src, st_utc_time_t* dst)
{
    if ( (src != NULL) && (dst != NULL) ) {
        dst->year         = src->year;
        dst->month        = src->month;
        dst->day          = src->day;
        dst->hour         = src->hour;
        dst->minute       = src->minute;
        dst->second       = src->second;
        dst->microseconds = src->microseconds;
        dst->zone.sign    = src->zone.sign;
        dst->zone.hrs     = src->zone.hrs;
        dst->zone.mins    = src->zone.mins;
    }
}
bool utcTime_toBuffer(const st_utc_time_t* utc_time, e_utc_format_t format,
                      uint8_t* buffer, uint16_t length)
{

    bool valid = utcTime_toBuffCheckParams(utc_time, format, buffer, length);

    if ( valid ) {
        valid = utcTime_isValidDateTime(utc_time->year, utc_time->month,
                                        utc_time->day, utc_time->hour,
                                        utc_time->minute, utc_time->second,
                                        utc_time->microseconds, utc_time->zone);
    }

    if ( valid ) {
        bool is_zone_z = utcTime_isZoneZ(&utc_time->zone);
        valid = utcTime_formatToBuffer(is_zone_z, utc_time, format, buffer,
                                       length);
    }

    return valid;
}

bool utcTime_isValidDate(uint16_t year, uint8_t month, uint8_t day)
{
    return dateTime_isValidDate(year, month, day);
}

bool utcTime_isValidDateTime(uint16_t year, uint8_t month, uint8_t day,
                             uint8_t hour, uint8_t minute, uint8_t second,
                             uint32_t microseconds, st_utc_zone_t zone)
{
    bool valid = utcTime_isTimeZoneValid(&zone);

    if ( valid ) {
        valid = dateTime_isValidDateTime(year, month, day, hour, minute, second,
                                         microseconds);
    }
    return valid;
}

bool utcTime_isValidUtc(const st_utc_time_t* utc_time)
{
    bool valid = (utc_time != NULL);

    if ( valid ) {
        valid = utcTime_isValidDateTime(utc_time->year, utc_time->month,
                                        utc_time->day, utc_time->hour,
                                        utc_time->minute, utc_time->second,
                                        utc_time->microseconds, utc_time->zone);
    }
    return valid;
}

bool utcTime_isTimeZoneValid(st_utc_zone_t* zone)
{
    uint32_t no_zones = sizeof(f_zone_Lut) / sizeof(f_zone_Lut[0]);
    bool valid        = false;
    if ( zone != NULL ) {
        for ( uint32_t ix = 0; ix < no_zones; ix++ ) {
            if ( (f_zone_Lut[ix].sign == zone->sign)
                 && (f_zone_Lut[ix].hrs == zone->hrs)
                 && (f_zone_Lut[ix].mins == zone->mins) ) {
                valid = true;
                break;
            }
        }
    }
    return valid;
}
void utcTime_setDudDate(st_utc_time_t* utc_time)
{
    if ( utc_time != NULL ) {
        /* clang-format off */
        utc_time->year         = 1999U;  utc_time->month        = 1U;
        utc_time->day          = 1U;     utc_time->hour         = 0U;
        utc_time->minute       = 0U;     utc_time->second       = 0U;
        utc_time->microseconds = 0U;     utc_time->zone.sign    = KEY_PLUS;
        utc_time->zone.hrs     = 0;      utc_time->zone.mins    = 0;
        /* clang-format on */
    }
}
