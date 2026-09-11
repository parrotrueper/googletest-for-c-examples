#ifndef UTCTIME_H__
#define UTCTIME_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#include "globaltypes.h"

/* text only lengths */
#define UTC_SECONDS_FORMAT_LEN_Z (20U) /* YYYY-MM-DDThh:mm:ssZ */
#define UTC_MS_FORMAT_LEN_Z      (24U) /* YYYY-MM-DDThh:mm:ss.sssZ */
#define UTC_SECONDS_FORMAT_LEN   (25U) /* YYYY-MM-DDThh:mm:ss+00:00 */
#define UTC_US_FORMAT_LEN_Z      (27U) /* YYYY-MM-DDThh:mm:ss.ssssssZ */
#define UTC_MS_FORMAT_LEN        (29U) /* YYYY-MM-DDThh:mm:ss.sss+00:00 */
#define UTC_US_FORMAT_LEN        (32U) /* YYYY-MM-DDThh:mm:ss.ssssss+00:00 */

/* minimum lengths for utcTime_toBuffer() buffers
 * function output includes a null terminator */
#define UTC_SECONDS_BUFF_LEN_Z                                                 \
    (UTC_SECONDS_FORMAT_LEN_Z + 1U) /* YYYY-MM-DDThh:mm:ssZ */
#define UTC_MS_BUFF_LEN_Z                                                      \
    (UTC_MS_FORMAT_LEN_Z + 1U) /* YYYY-MM-DDThh:mm:ss.sssZ */
#define UTC_SECONDS_BUFF_LEN                                                   \
    (UTC_SECONDS_FORMAT_LEN + 1U) /* YYYY-MM-DDThh:mm:ss+00:00 */
#define UTC_US_BUFF_LEN_Z                                                      \
    (UTC_US_FORMAT_LEN_Z + 1U) /* YYYY-MM-DDThh:mm:ss.ssssssZ */
#define UTC_MS_BUFF_LEN                                                        \
    (UTC_MS_FORMAT_LEN + 1U) /* YYYY-MM-DDThh:mm:ss.sss+00:00 */
#define UTC_US_BUFF_LEN                                                        \
    (UTC_US_FORMAT_LEN + 1U) /* YYYY-MM-DDThh:mm:ss.ssssss+00:00 */

/* UTC Valid offsets:
-12:00, -11:00, -10:00, -09:30, -09:00, -08:00, -07:00, -06:00, -05:00, -04:00
-03:30, -03:00, -02:00, -01:00, Z or +00:00, +01:00, +02:00, +03:00, +03:30,
+04:00, +04:30, +05:00, +05:30, +05:45, +06:00, +06:30, +07:00, +08:00, +08:45,
+09:00 +09:30, +10:00, +10:30, +11:00, +12:00, +12:45, +13:00, +14:00 */

/* timezone offset from UTC Z or +/- hh:mm */
typedef struct {
    uint8_t sign; /* 'Z' or '+' or '-' */
    uint8_t hrs;  /* e.g. -03:30 is 3 */
    uint8_t mins; /* e.g. -03:30 is 30 */
} st_utc_zone_t;

typedef struct {
    uint16_t year;         /* full year, e.g. 2026 */
    uint8_t month;         /* 1-12 */
    uint8_t day;           /* 1-31 */
    uint8_t hour;          /* 0-23 */
    uint8_t minute;        /* 0-59 */
    uint8_t second;        /* 0-59 */
    uint32_t microseconds; /* 0-999999 */
    st_utc_zone_t zone;    /* e.g. Z or -05:30 */
} st_utc_time_t;

typedef enum {
    eUtcSeconds,       /* YYYY-MM-DDThh:mm:ss+00:00 */
    eUtcSecondsZ,      /* as above but replaces +00:00 with Z */
    eUtcMilliseconds,  /* YYYY-MM-DDThh:mm:ss.sss+00:00 */
    eUtcMillisecondsZ, /* as above but replaces +00:00 with Z */
    eUtcMicroseconds,  /* YYYY-MM-DDThh:mm:ss.ssssss+00:00 */
    eUtcMicrosecondsZ, /* as above but replaces +00:00 with Z */
} e_utc_format_t;

/**
 * \brief convert a string to a UTC time structure
 * \param buffer null terminated string containing the UTC time data
 * \param length the length of the input string not counting the null terminator
 * valid lengths are 20,24,25,27,32.
 * \param utc_time output parameter, pointer to the st_utc_time_t structure to
 * populate
 * \return true if the conversion was successful
 * \note missing fields will be set to zero, e.g. if the string is
 * "2026-01-01T00:00:00Z" then the st_utc_time_t structure will have
 * year=2026, month=1, day=1, hour=0, minute=0, second=0, microseconds=0,
 * and zone.sign='Z', zone.hrs=0, zone.mins=0.
 */
bool utcTime_fromString(const uint8_t* buffer, uint16_t length,
                        st_utc_time_t* utc_time);

/**
 * \brief Compare two UTC time structures.
 * \param t1 First UTC time structure.
 * \param t2 Second UTC time structure.
 * \return true if t1 is greater than t2, false otherwise.
 */
bool utcTime_isGreater(const st_utc_time_t* t1, const st_utc_time_t* t2);

/**
 * \brief copy a UTC time structure from src to dst
 * \param src pointer to the source st_utc_time_t structure to copy from
 * \param dst pointer to the destination st_utc_time_t structure to copy to
 */
void utcTime_copy(const st_utc_time_t* src, st_utc_time_t* dst);
/**
 * \brief convert a UTC time structure to a byte buffer with a null terminator
 * \param utc_time pointer to the st_utc_time_t structure containing the UTC
 * time data
 * \param format the desired format for the output buffer (seconds, milliseconds,
 * or microseconds) for example
 *   - eUtcSeconds:        YYYY-MM-DDThh:mm:ss+00:00
 *   - eUtcMilliseconds:   YYYY-MM-DDThh:mm:ss.sss+00:00
 *   - eUtcMicroseconds:   YYYY-MM-DDThh:mm:ss.ssssss+00:00
 * \param buffer output byte buffer to store the UTC time data including a null
 * terminator
 * \param length the length of the output buffer, must be at least 33
 * bytes including the null terminator for microseconds format
 * \return true if the conversion was successful
 * The UTC time data will be written to the buffer in the following format:
 * YYYYMMDDThhmmssO (16 bytes ASCII)
 * - YYYY: year as 4 ASCII digits
 * - MM: month as 2 ASCII digits
 * - DD: day as 2 ASCII digits
 * - T: ASCII 'T' character
 * - hh: hour as 2 ASCII digits
 * - mm: minute as 2 ASCII digits
 * - ss: second as 2 ASCII digits
 * - .: ASCII '.' character (only for milliseconds and microseconds formats)
 * - sss: milliseconds as 3 ASCII digits (only for milliseconds and microseconds formats)
 * - ssssss: microseconds as 6 ASCII digits (only for microseconds format)
 * - O: timezone offset from UTC in the format: 'Z' (for +00:00) or +/-hh:mm
 */
bool utcTime_toBuffer(const st_utc_time_t* utc_time, e_utc_format_t format,
                      uint8_t* buffer, uint16_t length);


/**
 * \brief date helper, checks if the day is valid given the month and year.
 * \param year the full year, e.g. 2026
 * \param month the month, 1-12
 * \param day the day, 1-31 depending on the month and year
 * \return true if the date is valid, false otherwise
 */
bool utcTime_isValidDate(uint16_t year, uint8_t month, uint8_t day);
/**
 * \brief helper that checks date, time and zone are valid
 * \param year
 * \param month
 * \param day
 * \param hour
 * \param minute
 * \param second
 * \param microseconds
 * \param zone
 * \return true if all parameters are valid, false otherwise
 */
bool utcTime_isValidDateTime(uint16_t year, uint8_t month, uint8_t day,
                             uint8_t hour, uint8_t minute, uint8_t second,
                             uint32_t microseconds, st_utc_zone_t zone);

/**
 * \brief check whether a utc structure is valid
 * \param utc_time pointer to the st_utc_time_t structure to check
 * \return true if the utc structure is valid, false otherwise
 */
bool utcTime_isValidUtc(const st_utc_time_t* utc_time);

/**
 * \brief check whether a timezone is valid
 * \param zone pointer to the st_utc_zone_t structure to check
 * \return true if the timezone is valid, false otherwise
 */
bool utcTime_isTimeZoneValid(st_utc_zone_t* zone);

/**
 * \brief populate a utc structure with a dummy ISO 8601 date
 * 1999-01-01T00:00:00.000000+00:00
 * \param utc_time pointer to the st_utc_time_t structure to populate
 */
void utcTime_setDudDate(st_utc_time_t* utc_time);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* UTCTIME_H__ */
