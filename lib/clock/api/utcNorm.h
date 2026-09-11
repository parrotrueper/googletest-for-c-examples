/**
 * \file utcNorm.h
 * \brief UTC helper functions for normalising a UTC time structure to UTC Z
 * (zero offset)
 */
#ifndef UTCNORM_H__
#define UTCNORM_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#include "utcTime.h"

/**
 * \brief normalise a UTC time structure to UTC Z (zero offset)
 * \param utc_time pointer to the st_utc_time_t structure to normalise
 */
void utcNorm_toUtcZ(st_utc_time_t* utc_time);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* UTCNORM_H__ */
