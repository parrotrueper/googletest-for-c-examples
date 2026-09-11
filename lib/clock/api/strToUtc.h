#ifndef STRTOUTC_H__
#define STRTOUTC_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#include "utcTime.h"

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
bool strToUtc_convert(const uint8_t* buffer, uint16_t length,
                      st_utc_time_t* utc_time);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* STRTOUTC_H__ */
