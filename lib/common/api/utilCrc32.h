/**
 * \file util_Crc32.h
 * \brief Exposes interfaces that implement a CRC32 calculator
 */
#ifndef UTIL_CRC32_H__
#define UTIL_CRC32_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**
 * \brief Calculate CRC-32, polynomial 0x04C11DB7, seed 0xFFFFFFFF, reflected.
 * \param i_pData pointer to buffer data
 * \param in_length size of the buffer in bytes
 */
uint32_t util_Crc32_Calc(const uint8_t* i_pData, uint32_t in_length);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* UTIL_CRC32_H__ */
