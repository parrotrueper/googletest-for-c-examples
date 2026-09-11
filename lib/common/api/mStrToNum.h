#ifndef MSTRTONUM_H__
#define MSTRTONUM_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

#include "globaltypes.h"

/**
 * \brief convert a string to an int32_t
 * \param str pointer to the string to convert
 * \param length number of chars to check
 * \return true if the conversion was successful, false otherwise
 */
bool matoi_s32(const char_t* str, uint8_t length, int32_t* value);
/**
 * \brief convert a string to an uint32_t
 * \param str pointer to the string to convert
 * \param length number of chars to check
 * \return true if the conversion was successful, false otherwise
 */
bool matoi_u32(const char_t* str, uint8_t length, uint32_t* value);
/**
 * \brief convert a string to an uint16_t
 * \param str pointer to the string to convert
 * \param length number of chars to check
 * \return true if the conversion was successful, false otherwise
 */
bool matoi_u16(const char_t* str, uint8_t length, uint16_t* value);

/**
 * \brief convert a string to an uint8_t
 * \param str pointer to the string to convert
 * \param length number of chars to check
 * \return true if the conversion was successful, false otherwise
 */
bool matoi_u8(const char_t* str, uint8_t length, uint8_t* value);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MSTRTONUM_H__ */
