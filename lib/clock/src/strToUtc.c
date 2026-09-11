#include <stddef.h>

#include "dateTime.h"
#include "libAscii.h"
#include "mStrToNum.h"
#include "strToUtc.h"

static uint8_t f_utc_str_len_Lut[] = {
    UTC_SECONDS_FORMAT_LEN_Z, UTC_MS_FORMAT_LEN_Z, UTC_SECONDS_FORMAT_LEN,
    UTC_US_FORMAT_LEN_Z,      UTC_MS_FORMAT_LEN,   UTC_US_FORMAT_LEN};
/*******************************************************************************
 *                         Private Functions
 ******************************************************************************/
/** \brief initialise a st_utc_time_t structure to zero */
static void strToUtc_initStruct(st_utc_time_t* utc_time)
{ /* clang-format off */
    utc_time->year         = 0U;  utc_time->month        = 0U;
    utc_time->day          = 0U;  utc_time->hour         = 0U;
    utc_time->minute       = 0U;  utc_time->second       = 0U;
    utc_time->microseconds = 0U;  utc_time->zone.sign    = KEY_UC_Z;
    utc_time->zone.hrs     = 0U;  utc_time->zone.mins    = 0U;
    /* clang-format on */
}
/*-------------------------- ZONE --------------------------------------------*/
/* based on the string length get the index for the sign */
static uint8_t strToUtc_getZoneSignIx(uint16_t length)
{
    uint8_t index = 0;
    switch ( length ) {
        case UTC_SECONDS_FORMAT_LEN_Z:
        case UTC_SECONDS_FORMAT_LEN:
            index = 19U;
            break;
        case UTC_MS_FORMAT_LEN_Z:
        case UTC_MS_FORMAT_LEN:
            index = 23;
            break;
        case UTC_US_FORMAT_LEN_Z:
        case UTC_US_FORMAT_LEN:
            index = 26;
            break;
        default:   /* LCOV_EXCL_LINE */
            break; /* LCOV_EXCL_LINE */
    }
    return index;
}
/* get zone +/- hh:mm style */
static bool strToUtc_getZoneFull(const uint8_t* buffer, uint16_t length,
                                 st_utc_time_t* utc_time)
{
    uint8_t ix = strToUtc_getZoneSignIx(length);
    bool valid = ((buffer[ix] == KEY_PLUS) || (buffer[ix] == KEY_DASH));

    if ( valid ) {
        utc_time->zone.sign = buffer[ix];
    }
    /* hrs */
    if ( valid ) {
        ix++;
        valid = matoi_u8((const char_t*)(&buffer[ix]), 2, &utc_time->zone.hrs);
    }
    /* mins */
    if ( valid ) {
        ix += 3U; /* hh: */
        valid = matoi_u8((const char_t*)(&buffer[ix]), 2, &utc_time->zone.mins);
    }
    /* validate */
    if ( valid ) {
        valid = utcTime_isTimeZoneValid(&utc_time->zone);
    }

    return valid;
}
/* get zone KEY_UC_Z */
static bool strToUtc_getZoneZ(const uint8_t* buffer, uint16_t length,
                              st_utc_time_t* utc_time)
{
    bool valid = false;
    uint8_t ix = strToUtc_getZoneSignIx(length);
    if ( buffer[ix] == KEY_UC_Z ) {
        utc_time->zone.sign = KEY_UC_Z;
        utc_time->zone.hrs  = 0;
        utc_time->zone.mins = 0;
        valid               = true;
    }
    return valid;
}

/* based on the string length determine if it is Zulu time */
static bool strToUtc_isZoneZ(uint16_t length)
{
    return ((length == UTC_SECONDS_FORMAT_LEN_Z)
            || (length == UTC_MS_FORMAT_LEN_Z)
            || (length == UTC_US_FORMAT_LEN_Z));
}
static bool strToUtc_getZone(const uint8_t* buffer, uint16_t length,
                             st_utc_time_t* utc_time)
{
    bool valid        = true;
    bool done_parsing = false;

    /* check for Zulu first */
    bool is_Z = strToUtc_isZoneZ(length);
    if ( is_Z ) {
        valid        = strToUtc_getZoneZ(buffer, length, utc_time);
        done_parsing = true;
    }
    /* sign & hours */
    if ( valid && !done_parsing ) {
        valid = strToUtc_getZoneFull(buffer, length, utc_time);
    }

    return valid;
}
/*-------------------------- TIME --------------------------------------------*/
/* us/ms */
static bool strToUtc_getSubseconds(const uint8_t* buffer, uint16_t length,
                                   st_utc_time_t* utc_time)
{
    bool valid      = true;
    uint8_t bytes   = 0;
    uint32_t factor = 1U;
    bool process    = false;
    if ( (length == UTC_US_FORMAT_LEN) || (length == UTC_US_FORMAT_LEN_Z) ) {
        bytes   = 6U;
        process = true;
    }
    if ( (length == UTC_MS_FORMAT_LEN) || (length == UTC_MS_FORMAT_LEN_Z) ) {
        bytes   = 3U;
        factor  = 1000U;
        process = true;
    }
    if ( process ) {
        uint32_t microseconds = 0U;
        valid = matoi_u32((const char_t*)(&buffer[20]), bytes, &microseconds);
        if ( valid ) {
            utc_time->microseconds = microseconds * factor;
        }
    }
    return valid;
}

/* hh:mm */
static bool strToUtc_getTime(const uint8_t* buffer, uint16_t length,
                             st_utc_time_t* utc_time)
{

    bool valid = matoi_u8((const char_t*)(&buffer[11]), 2, &utc_time->hour);
    if ( valid ) {
        valid = matoi_u8((const char_t*)(&buffer[14]), 2, &utc_time->minute);
    }
    if ( valid ) {
        valid = matoi_u8((const char_t*)(&buffer[17]), 2, &utc_time->second);
    }
    /* parse optional fractional seconds if present */
    if ( valid ) {
        valid = strToUtc_getSubseconds(buffer, length, utc_time);
    }
    if ( valid ) {
        valid = dateTime_isValidTime(utc_time->hour, utc_time->minute,
                                     utc_time->second, utc_time->microseconds);
    }
    return valid;
}
/*-------------------------- DATE --------------------------------------------*/
static bool strToUtc_getYear(const uint8_t* buffer, st_utc_time_t* utc_time)
{
    uint16_t year = 0U;
    bool valid    = matoi_u16((const char_t*)buffer, 4, &year);
    if ( valid ) {
        utc_time->year = year;
    }
    return valid;
}
static bool strToUtc_getDate(const uint8_t* buffer, st_utc_time_t* utc_time)
{
    bool valid = strToUtc_getYear(buffer, utc_time);
    if ( valid ) {
        valid = matoi_u8((const char_t*)(&buffer[5]), 2, &utc_time->month);
    }
    if ( valid ) {
        valid = matoi_u8((const char_t*)(&buffer[8]), 2, &utc_time->day);
    }
    if ( valid ) {
        valid = dateTime_isValidDate(utc_time->year, utc_time->month,
                                     utc_time->day);
    }
    return valid;
}
/*-------------------------- DELIMITER CHECKS --------------------------------*/
static uint8_t strToUtc_getSecondColonIx(uint16_t length)
{
    uint8_t ix = 16; /* default to first ':' */
    switch ( length ) {
        case UTC_SECONDS_FORMAT_LEN:
            ix = 22;
            break;
        case UTC_MS_FORMAT_LEN:
            ix = 26;
            break;
        case UTC_US_FORMAT_LEN:
            ix = 29;
            break;
        default: /* unhandled length */
            break;
    }
    return ix;
}
/* check the ':' */
/** \brief check if char is ':' */
static bool strToUtc_isColon(const char_t colon)
{
    return (colon == (char_t)(KEY_COLON));
}
static bool strToUtc_checkColons(const uint8_t* buffer, uint16_t length)
{
    uint8_t ix = 13; /* hr: */
    bool valid = strToUtc_isColon((const char_t)(buffer[ix]));

    ix = 16; /* mm:*/
    if ( valid ) {
        valid = strToUtc_isColon((const char_t)(buffer[ix]));
    }
    /* zone : */
    if ( valid ) {
        ix = strToUtc_getSecondColonIx(length);
    }
    if ( valid && (ix != 16U) ) {
        valid = strToUtc_isColon((const char_t)(buffer[ix]));
    }
    return valid;
}
/* check the '-' */
static bool strToUtc_checkHyphens(const uint8_t* buffer)
{
    return (buffer[4] == KEY_DASH) && (buffer[7] == KEY_DASH);
}
/** \brief checks that the string has the 'T', ':' and '-' in the right place */
static bool strToUtc_checkDelimiters(const uint8_t* buffer, uint16_t length)
{
    bool valid = strToUtc_checkHyphens(buffer);

    if ( valid ) {
        valid = (buffer[10] == KEY_UC_T);
    }
    if ( valid ) {
        valid = strToUtc_checkColons(buffer, length);
    }
    return valid;
}
/** \brief checks that the string length matches one of the valid UTC formats */
static bool strToUtc_checkStrLength(uint16_t length)
{
    /* valid lengths */
    uint8_t lengths = (uint8_t)(sizeof(f_utc_str_len_Lut)
                                / sizeof(f_utc_str_len_Lut[0]));
    bool valid      = false;
    for ( uint8_t ix = 0U; ix < lengths; ++ix ) {
        if ( length == f_utc_str_len_Lut[ix] ) {
            valid = true;
            break;
        }
    }
    return valid;
}
/** \brief sanity check input parameters */
static bool strToUtc_checkParams(const uint8_t* buffer, uint16_t length,
                                 st_utc_time_t* utc_time)
{
    return (buffer != NULL) && (utc_time != NULL)
           && strToUtc_checkStrLength(length);
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
bool strToUtc_convert(const uint8_t* buffer, uint16_t length,
                      st_utc_time_t* utc_time)
{
    bool valid = strToUtc_checkParams(buffer, length, utc_time);

    if ( valid ) {
        strToUtc_initStruct(utc_time);
        valid = strToUtc_checkDelimiters(buffer, length);
    }
    if ( valid ) {
        valid = strToUtc_getDate(buffer, utc_time);
    }
    if ( valid ) {
        valid = strToUtc_getTime(buffer, length, utc_time);
    }
    if ( valid ) {
        valid = strToUtc_getZone(buffer, length, utc_time);
    }
    if ( valid ) {
        valid = utcTime_isValidUtc(utc_time);
    }

    return valid;
}
