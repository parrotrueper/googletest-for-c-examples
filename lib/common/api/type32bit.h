/**
 * \file type32bit.h
 * \brief exposes interfaces to convert to and from a type to a 32-bit
 * representation (all conversions in Little Endian)
 */
#ifndef TYPE32BIT_H__
#define TYPE32BIT_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief convert a uint16_t, uint8_t, uint8_t to a 32-bit representation
 * \param in_high - the high 16 bits of the 32-bit representation
 * \param in_mid - the middle 8 bits of the 32-bit representation
 * \param in_low - the low 8 bits of the 32-bit representation
 * \return the 32-bit representation in Little Endian
 */
uint32_t type32bit_fromU16U8U8(uint16_t in_high, uint8_t in_mid,
                               uint8_t in_low);
/**
 * \brief convert 4 uint8_t values to a 32-bit representation
 * \param in_byte3 - the highest 8 bits of the 32-bit representation
 * \param in_byte2 - the next 8 bits of the 32-bit representation
 * \param in_byte1 - the next 8 bits of the 32-bit representation
 * \param in_byte0 - the lowest 8 bits of the 32-bit representation
 * \return the 32-bit representation in Little Endian
 */
uint32_t type32bit_fromU8U8U8U8(uint8_t in_byte3, uint8_t in_byte2,
                                uint8_t in_byte1, uint8_t in_byte0);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TYPE32BIT_H__ */
