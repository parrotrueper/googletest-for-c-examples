/**
 * \file mString.h
 * \brief exposes MISRA compliant alternatives for string.h helper functions
 * that fall into the undefined behaviour category.
 */
#ifndef mString_H__
#define mString_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <stdbool.h>
#include <stdint.h>

#include "globaltypes.h"

/*lint -esym(755,mString_UINT32_LEN) referenced by test code */
/*lint -esym(755,mString_INT32_LEN) referenced by errorHandler.c */
/*lint -esym(755,mString_FLOAT64_LEN) referenced by some_other_file.c */

#define mString_UINT32_LEN  (11)
#define mString_INT32_LEN   (12)
#define mString_FLOAT64_LEN (23)

/**
 * \brief Find the length of a string
 * \param i_str - pointer to the null-terminated byte string to be examined
 * \param i_strsz - maximum number of characters to examine
 * \return the length of the string or 0 on error
 *          - the pointer was NULL
 *          - the null terminator was not found
 *          - the first character was the null terminator
 *          - i_strsz is out of bounds
 */
uint32_t m_strlen_s(const char* i_str, uint32_t i_strsz);

/**
 * Appends a copy of the null-terminated byte string pointed to by src to the
 * end of the null-terminated byte string pointed to by dest. The character
 * src[0] replaces the null terminator at the end of dest. The resulting byte
 * string is null-terminated.
 *
 * If the space required by src exceeds the space available in dest, dest
 * is left unchanged.
 *
 * \param i_dest - pointer to the null-terminated byte string to append to
 * \param i_destsz 	- the size of the destination buffer
 * \param i_src  - pointer to the null-terminated byte string to copy from
 * \return the length of the result string (not including null terminator) or 0
 *         on error:
 *          - parameters are invalid
 *          - the null terminator was not found in the input string
 *          - the result string did not fit in the destination
 */
uint32_t m_strcat_s(char* i_dest, uint32_t i_destsz, const char* i_src);

/**
 * \brief Convert int32 to string
 * \param i_val - int32 to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 12 bytes)
 * \return the length of the result string or 0 on error
 */
uint32_t m_itoa_s(const int32_t i_val, char* i_dest, uint32_t i_strsz);

/**
 * \brief Convert uint32 to decimal number string
 * \param i_val - uint32 to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 11 bytes)
 * \return the length of the result string or 0 on error
 */
uint32_t m_utoa_s(const uint32_t i_val, char* i_dest, uint32_t i_strsz);
uint32_t m_utoa_s_pad(const uint32_t i_val, char* i_dest, uint32_t i_strsz,
                      uint32_t i_min_width, bool i_zero_pad);
uint32_t m_itoa_s_pad(const int32_t i_val, char* i_dest, uint32_t i_strsz,
                      uint32_t i_min_width, bool i_zero_pad);
uint32_t m_utoaHex_s(uint32_t i_val, char* i_dest, uint32_t i_strsz,
                     uint32_t i_min_digits);

/**
 * \brief Converts a 64-bit float to ASCII in xxx.xxx format
 * \param i_val the float to convert
 * \param i_dest - buffer for the string
 * \param i_strsz - number of bytes available in the buffer, minimum 23
 * \param i_pr - precision, maximum value is 10
 */
uint32_t m_ftoa_s(float64_t i_val, char* i_dest, uint32_t i_strsz,
                  uint32_t i_pr);

/**
 * \brief Find the first substring in string
 * \param i_str - pointer to a null terminated string to be examined
 * \param i_strsz - length of string to be searched
 * \param i_substr - pointer to the substring to match
 * \param i_subsz - length of substring to search for
 * \return pointer to the first character of the found substring in i_str, or a
 * null pointer if such substring is not found or on error
 */
const char* m_strstr_s(const char* i_str, uint32_t i_strsz,
                       const char* i_substr, uint32_t i_subsz);

/**
 * \brief compare two strings for equality
 * \param i_str1 - pointer to the first null terminated string
 * \param i_str2 - pointer to the second null terminated string
 * \param i_strsz - maximum number of characters to compare
 * \return 0 if the strings are equal, non-zero otherwise
 */
uint32_t m_strcmp_s(const char* i_str1, const char* i_str2, uint32_t i_strsz);

/**
 * \brief Convert uint8 to hex string
 * \param value - number to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 2 bytes)
 * \return the length of the result string or 0 on error
 * \note if the destination buffer is larger than 2 bytes, a null terminator
 * will be added
 */
uint32_t m_utoa8Hex_s(const uint8_t value, char* i_dest, uint32_t i_strsz);
uint32_t m_uintptrHex_s(uintptr_t i_val, char* i_dest, uint32_t i_strsz,
                        uint32_t i_min_digits);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* mString_H__*/
