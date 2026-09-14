/**
 * \file calcCrc32.h
 * \brief Exposes interfaces that implement a CRC32 calculator
 */
#ifndef calcCrc32_H__
#define calcCrc32_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**
 * \brief Calculate CRC-32, polynomial 0x04C11DB7, seed 0xFFFFFFFF, reflected.
 * \param in_data pointer to buffer data
 * \param in_length size of the buffer in bytes
 */
uint32_t calcCrc32(const uint8_t* in_data, uint32_t in_length);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* calcCrc32_H__ */
