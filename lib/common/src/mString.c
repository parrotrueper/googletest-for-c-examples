/**
 * \file mString.c
 * \brief MISRA compliant alternatives for string.h helper functions
 * that fall into the undefined behaviour category.
 */
#include <stddef.h>

#include "mString.h"

#define M_STR_RADIX              (10U)
#define M_STR_ASCII_0            ('0') /*(char)(0x30))*/
#define M_STR_S32_CHARS          (12U) /*   '-2147483648' + '\0' */
#define M_STR_U32_CHARS          (11U) /*   '4294967295'  + '\0' */
#define M_STR_UINTPTR_HEX_DIGITS (2U * sizeof(uintptr_t))
#define M_STR_RESULT_OK          (0U)

#define M_STR_ZERO_U8 ((uint8_t)(0x30)) /* '0' */
#define M_STR_UCA_U8  ((uint8_t)(0x41)) /* 'A' */

#define M_STR_F64_0 (float64_t)(0.0)

#define M_STR_FLT_PRECISION (10U)
static const float64_t f_f64LutRounder[M_STR_FLT_PRECISION + 1U] = {
    (float64_t)(0.5),           /* 0  */
    (float64_t)(0.05),          /* 1  */
    (float64_t)(0.005),         /* 2  */
    (float64_t)(0.0005),        /* 3  */
    (float64_t)(0.00005),       /* 4  */
    (float64_t)(0.000005),      /* 5  */
    (float64_t)(0.0000005),     /* 6  */
    (float64_t)(0.00000005),    /* 7  */
    (float64_t)(0.000000005),   /* 8  */
    (float64_t)(0.0000000005),  /* 9  */
    (float64_t)(0.00000000005), /* 10 */
};

/**
 * \brief convert a byte nibble to a hex character
 */
static char mstring_nibbleToHex(uint8_t value)
{
    uint8_t result = value;
    uint8_t ten    = 10U;
    if ( value < ten ) {
        result += M_STR_ZERO_U8;
    } else {
        result -= ten;
        result += M_STR_UCA_U8;
    }
    return (char)(result);
}

/**
 * \brief Find the length of a string
 * \param i_str - pointer to the null-terminated byte string to be examined
 * \param i_strsz - maximum number of characters to examine
 * \return the length of the string or 0 on error
 *          - the pointer was NULL
 *          - the null terminator was not found
 *          - the first character was the null terminator
 *          - i_strsz > max value of i_strsz type in this case max value of
 * uint32_t
 */
uint32_t m_strlen_s(const char* i_str, uint32_t i_strsz)
{
    uint32_t result = M_STR_RESULT_OK;

    if ( (i_str != NULL) && (i_str[0] != '\0') ) {
        for ( uint32_t uiIx = 0U; uiIx < i_strsz; uiIx++ ) {
            /*lint -e661 we rely on the caller passing the correct bounds for the buffer */
            if ( i_str[uiIx] == '\0' ) {
                result = uiIx;
                break;
            }
        }
    }

    return result;
}

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
 * \return the length of the result string or 0 on error
 *          - parameters are invalid
 *          - the null terminator was not found in the input string
 *          - the result string did not fit in the destination
 */
uint32_t m_strcat_s(char* i_dest, uint32_t i_destsz, const char* i_src)
{
    uint32_t result = M_STR_RESULT_OK;
    uint32_t uiDlen;
    uint32_t uiSlen;
    uint32_t uiJx;
    uint32_t uiIx;

    if ( (i_dest != NULL) && (i_destsz > 1U) && (i_src != NULL) ) {
        uiDlen = m_strlen_s(i_dest, i_destsz);
        uiSlen = m_strlen_s(i_src, i_destsz);

        if ( (uiDlen + uiSlen) <= (i_destsz - 1U) ) {
            uiIx = uiDlen;
            for ( uiJx = 0; uiJx < uiSlen; uiJx++ ) {
                i_dest[uiIx] = i_src[uiJx];
                uiIx++;
            }
            i_dest[uiIx] = '\0';
            result       = uiIx;
        }
    }
    return result;
}

/**
 * \brief Convert int32 to decimal number string
 * \param i_val - int to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 12 bytes)
 * \return the length of the result string or 0 on error
 */
uint32_t m_itoa_s(const int32_t i_val, char* i_dest, uint32_t i_strsz)
{
    /* #lizard forgive this function, coded for speed so reluctant to break it up*/
    uint32_t result = M_STR_RESULT_OK;
    char tmpStr[M_STR_S32_CHARS];
    char* pTmp      = &tmpStr[0];
    int32_t s32Val  = (i_val < 0) ? (0 - i_val) : i_val;
    uint32_t u32Val = (uint32_t)(s32Val);

    if ( (i_dest != NULL) && (i_strsz >= M_STR_S32_CHARS) ) {
        if ( u32Val == 0U ) {
            i_dest[result] = M_STR_ASCII_0;
            result++;
        } else {
            do {
                /*lint -e9033 we are converting from a number to a char
                so accept the loss of information. '+' moves along the ASCII
                table */
                *pTmp = (char)((u32Val % (uint32_t)(M_STR_RADIX))
                               + (uint32_t)(M_STR_ASCII_0));
                pTmp++;
                u32Val /= M_STR_RADIX;
            } while ( u32Val > 0U );
            if ( i_val < 0 ) {
                *pTmp = '-';
                pTmp++;
            }
            pTmp--;
            /* swap order and save */
            while ( pTmp != &tmpStr[0] ) {
                i_dest[result] = *pTmp;
                result++;
                pTmp--;
            }
            i_dest[result] = *pTmp;
            result++;
        }
        i_dest[result] = '\0';
    }
    return result;
}

/**
 * \brief Convert uint32 to decimal number string
 * \param i_val - number to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 11 bytes)
 * \return the length of the result string or 0 on error
 */
uint32_t m_utoa_s(const uint32_t i_val, char* i_dest, uint32_t i_strsz)
{
    uint32_t result = M_STR_RESULT_OK;
    char tmpStr[M_STR_U32_CHARS];
    char* pTmp      = &tmpStr[0];
    uint32_t u32Val = i_val;

    if ( (i_dest != NULL) && (i_strsz >= M_STR_U32_CHARS) ) {
        if ( u32Val == 0U ) {
            i_dest[result] = M_STR_ASCII_0;
            result++;
        } else {
            do {
                *pTmp = (char)((u32Val % (uint32_t)(M_STR_RADIX))
                               + (uint32_t)(M_STR_ASCII_0));
                pTmp++;
                u32Val /= M_STR_RADIX;
            } while ( u32Val > 0U );
            pTmp--;
            /* swap order and save */
            while ( pTmp != &tmpStr[0] ) {
                i_dest[result] = *pTmp;
                result++;
                pTmp--;
            }
            i_dest[result] = *pTmp;
            result++;
        }
        i_dest[result] = '\0';
    }
    return result;
}

uint32_t m_utoa_s_pad(const uint32_t i_val, char* i_dest, uint32_t i_strsz,
                      uint32_t i_min_width, bool i_zero_pad)
{
    /*#lizard forgives this function */
    uint32_t result = m_utoa_s(i_val, i_dest, i_strsz);
    if ( (result > 0U) && (i_zero_pad) && (result < i_min_width) ) {
        uint32_t pad = i_min_width - result;
        if ( i_strsz > result ) {
            char tmp[M_STR_U32_CHARS];
            uint32_t ix;
            for ( ix = 0U; ix < pad; ix++ ) {
                tmp[ix] = '0';
            }
            for ( ix = 0U; ix < result; ix++ ) {
                tmp[pad + ix] = i_dest[ix];
            }
            if ( (pad + result) < i_strsz ) {
                tmp[pad + result] = '\0';
                for ( ix = 0U; ix <= (pad + result); ix++ ) {
                    i_dest[ix] = tmp[ix];
                }
                result += pad;
            } else {
                result = M_STR_RESULT_OK;
            }
        }
    }
    return result;
}

uint32_t m_itoa_s_pad(const int32_t i_val, char* i_dest, uint32_t i_strsz,
                      uint32_t i_min_width, bool i_zero_pad)
{
    /*#lizard forgives this function */
    uint32_t result = m_itoa_s(i_val, i_dest, i_strsz);
    if ( (result > 0U) && (i_zero_pad) && (result < i_min_width) ) {
        uint32_t pad  = i_min_width - result;
        bool negative = (i_val < 0);
        if ( i_strsz > result ) {
            char tmp[M_STR_S32_CHARS];
            uint32_t ix;
            uint32_t digitStart = negative ? 1U : 0U;
            if ( negative ) {
                tmp[0] = i_dest[0];
            }
            for ( ix = 0U; ix < pad; ix++ ) {
                tmp[digitStart + ix] = '0';
            }
            for ( ix = digitStart; ix < result; ix++ ) {
                tmp[digitStart + pad + ix - digitStart] = i_dest[ix];
            }
            if ( (digitStart + pad + (result - digitStart)) < i_strsz ) {
                tmp[digitStart + pad + (result - digitStart)] = '\0';
                for ( ix = 0U; ix <= (digitStart + pad + (result - digitStart));
                      ix++ ) {
                    i_dest[ix] = tmp[ix];
                }
                result += pad;
            } else {
                result = M_STR_RESULT_OK;
            }
        }
    }
    return result;
}

static uint32_t mString_hexToString(uintptr_t i_val, char* i_dest,
                                    uint32_t i_strsz, uint32_t i_min_digits)
{
    /*#lizard forgives this function */
    uint32_t result = M_STR_RESULT_OK;
    char tmpStr[M_STR_UINTPTR_HEX_DIGITS];
    uint32_t idx   = 0U;
    uintptr_t uVal = i_val;

    if ( (i_dest != NULL) && (i_strsz > 0U)
         && (i_min_digits <= M_STR_UINTPTR_HEX_DIGITS) ) {
        if ( uVal == (uintptr_t)0 ) {
            tmpStr[idx++] = '0';
        } else {
            while ( (uVal != (uintptr_t)0)
                    && (idx < M_STR_UINTPTR_HEX_DIGITS) ) {
                uint32_t digit = (uint32_t)(uVal & (uintptr_t)0xFU);
                tmpStr[idx++]  = (char)((digit < 10U)
                                            ? (0x30U + digit) /* '0' */
                                            : (0x61U + (digit - 10U))); /* 'a' */
                uVal >>= 4U;
            }
        }

        while ( idx < i_min_digits ) {
            tmpStr[idx++] = '0';
        }

        if ( idx < i_strsz ) {
            uint32_t rev;
            for ( rev = 0U; rev < idx; rev++ ) {
                i_dest[rev] = tmpStr[idx - rev - 1U];
            }
            i_dest[idx] = '\0';
            result      = idx;
        }
    }
    return result;
}

uint32_t m_utoaHex_s(uint32_t i_val, char* i_dest, uint32_t i_strsz,
                     uint32_t i_min_digits)
{
    return mString_hexToString((uintptr_t)i_val, i_dest, i_strsz, i_min_digits);
}

uint32_t m_uintptrHex_s(uintptr_t i_val, char* i_dest, uint32_t i_strsz,
                        uint32_t i_min_digits)
{
    return mString_hexToString(i_val, i_dest, i_strsz, i_min_digits);
}

/**
 * \brief Converts a 64-bit float to ASCII in xxx.xxx format
 * \param i_val the float to convert
 * \param i_dest - buffer for the string
 * \param i_strsz - number of bytes available in the buffer, minimum 23
 * \param i_pr - precision, maximum value is 10
 */
uint32_t m_ftoa_s(float64_t i_val, char* i_dest, uint32_t i_strsz,
                  uint32_t i_pr)
{
    /* #lizard forgive this function, coded for speed so reluctant to break it up*/
    char tmpStr[12];
    char* pTmp        = &tmpStr[0];
    uint32_t uiIx     = 0;
    float64_t f64PosV = (i_val < M_STR_F64_0) ? -i_val : i_val;
    uint32_t u32Prs = (i_pr > M_STR_FLT_PRECISION) ? M_STR_FLT_PRECISION : i_pr;

    if ( (i_dest != NULL) && (i_strsz >= 23U) ) {
        if ( i_val < M_STR_F64_0 ) {
            i_dest[uiIx] = '-';
            uiIx++;
        }
        if ( u32Prs != 0U ) {
            f64PosV += f_f64LutRounder[u32Prs]; /* round up */
        }
        uint64_t u64IntPart = (uint64_t)(f64PosV); /* integer part */
        f64PosV -= (float64_t)(u64IntPart);        /* decimal part*/

        if ( u64IntPart == 0U ) {
            i_dest[uiIx] = M_STR_ASCII_0;
            uiIx++;
        } else {
            do {
                *pTmp = (char)((u64IntPart % (uint64_t)(M_STR_RADIX))
                               + (uint64_t)(M_STR_ASCII_0));
                pTmp++;
                u64IntPart /= 10U;
            } while ( u64IntPart > 0U );
            pTmp--;
            /* swap order and save */
            while ( pTmp != &tmpStr[0] ) {
                i_dest[uiIx] = *pTmp;
                uiIx++;
                pTmp--;
            }
            i_dest[uiIx] = *pTmp;
            uiIx++;
        }
        i_dest[uiIx] = '.'; /* add decimal point */
        uiIx++;
        do {
            f64PosV *= (float64_t)(10.0);
            i_dest[uiIx] = (char)((uint8_t)(M_STR_ASCII_0)
                                  + (uint8_t)(f64PosV));
            uiIx++;
            uint8_t u8Dig = (uint8_t)(f64PosV);
            f64PosV -= (float64_t)(u8Dig);
            u32Prs--;
        } while ( u32Prs > 0U );
        i_dest[uiIx] = '\0';
    }
    return uiIx;
}

/**
 * \brief strstr helper chacks for nulls on input parameters
 * \param i_str - pointer to a null terminated string to be examined
 * \param i_substr - pointer to the substring to match
 * \returns 0 if all OK, 1 for errors found
 */
static inline uint32_t m_strstr_param_areNulls(const char* i_str,
                                               const char* i_substr)
{
    uint32_t done = 0;

    if ( (i_str == NULL) || (i_substr == NULL) ) {
        done = 1U;
    } else {
        if ( (*i_str == '\0') || (*i_substr == '\0') ) {
            done = 1U;
        }
    }
    return done;
}

/**
 * \brief strstr helper validates string lengths
 * \param i_str - pointer to a null terminated string to be examined
 * \param i_strsz - length of string to be searched
 * \param i_substr - pointer to the substring to match
 * \param i_subsz - length of substring to search for
 * \returns 0 if all OK, 1 for errors found
 */
static inline uint32_t m_strstr_paramCheckLengths(const char* i_str,
                                                  uint32_t i_strsz,
                                                  const char* i_substr,
                                                  uint32_t i_subsz)
{
    uint32_t done = 1U;

    uint32_t uiStrLen = m_strlen_s(i_str, i_strsz + 1U);
    uint32_t uiSubLen = m_strlen_s(i_substr, i_subsz + 1U);
    if ( (i_strsz == uiStrLen) && (i_subsz == uiSubLen) ) {
        done = 0U;
    }
    return done;
}
/**
 * \brief strstr helper validates input parameters
 * \param i_str - pointer to a null terminated string to be examined
 * \param i_strsz - length of string to be searched
 * \param i_substr - pointer to the substring to match
 * \param i_subsz - length of substring to search for
 * \returns 0 if all OK, 1 for errors found
 */
static inline uint32_t m_strstr_param_validate(const char* i_str,
                                               uint32_t i_strsz,
                                               const char* i_substr,
                                               uint32_t i_subsz)
{
    uint32_t done = m_strstr_param_areNulls(i_str, i_substr);

    if ( (i_strsz == 0U) || (i_subsz == 0U) || (i_strsz < i_subsz) ) {
        done = 1U;
    }

    if ( done == 0U ) {
        done = m_strstr_paramCheckLengths(i_str, i_strsz, i_substr, i_subsz);
    }
    return done;
}
/**
 * \brief strstr helper checks if both pointers are the same
 * \returns the pointer if they are the same or NULL if not
 */
static inline const char* m_strstr_arePointersEqual(const char* i_str,
                                                    const char* i_substr)
{
    const char* p_result = NULL;
    if ( i_str == i_substr ) {
        p_result = i_str;
    }
    return p_result;
}
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
                       const char* i_substr, uint32_t i_subsz)
{
    uint32_t done = 0;

    const char* p_result = NULL;

    /* check for invalid parameters */
    done = m_strstr_param_validate(i_str, i_strsz, i_substr, i_subsz);

    if ( done == 0U ) {
        p_result = m_strstr_arePointersEqual(i_str, i_substr);
    }
    if ( (p_result == NULL) && (done == 0U) ) {
        uint32_t uiFound = 0;
        /*#lizard forgives this nested for loop */
        for ( uint32_t uiI = 0; uiI < (i_strsz - i_subsz + 1U); uiI++ ) {
            for ( uint32_t uiJ = 0; uiJ < i_subsz; uiJ++ ) {
                uiFound = 1; /* reset the flag before comparing */
                if ( i_str[uiI + uiJ] != i_substr[uiJ] ) {
                    uiFound = 0;
                    break;
                }
            }
            if ( uiFound == 1U ) {
                p_result = &i_str[uiI];
                break;
            }
        }
    }

    return p_result;
}

/**
 * \brief compare two strings for equality
 * \param i_str1 - pointer to the first null terminated string
 * \param i_str2 - pointer to the second null terminated string
 * \param i_strsz - maximum number of characters to compare
 * \return 0 if the strings are equal, non-zero otherwise
 */
uint32_t m_strcmp_s(const char* i_str1, const char* i_str2, uint32_t i_strsz)
{
    uint32_t result = 1; /* assume not equal */

    /*#lizard forgives this nested for loop */
    if ( (i_str1 != NULL) && (i_str2 != NULL) ) {
        for ( uint32_t uiIx = 0U; uiIx < (i_strsz + 1U); uiIx++ ) {
            if ( i_str1[uiIx] != i_str2[uiIx] ) {
                result = 1; /* not equal */
                break; /*lint -e{9011} multiple exit loops is easier to read*/
            }
            if ( (i_str1[uiIx] == '\0') && (i_str2[uiIx] == '\0') ) {
                result = 0; /* equal */
                break; /*lint -e{9011} multiple exit loops is easier to read*/
            }
        }
    }

    return result;
}
/**
 * \brief Convert uint8 to hex string
 * \param value - number to be converted
 * \param i_dest - pointer to the destination buffer
 * \param i_strsz  - the size of the destination buffer (minimum is 2 bytes)
 * \return the length of the result string or 0 on error
 * \note if the destination buffer is larger than 2 bytes, a null terminator
 * will be added
 */
uint32_t m_utoa8Hex_s(const uint8_t value, char* i_dest, uint32_t i_strsz)
{
    uint32_t result = M_STR_RESULT_OK;

    if ( (i_dest != NULL) && (i_strsz >= 2U) ) {
        i_dest[result] = mstring_nibbleToHex((uint8_t)(value >> 4U));
        result++;
        i_dest[result] = mstring_nibbleToHex((uint8_t)(value & 0x0FU));
        if ( i_strsz > 2U ) {
            result++;
            i_dest[result] = '\0';
        }
    }
    return result;
}
