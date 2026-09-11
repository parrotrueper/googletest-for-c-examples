/**
 * \file mPrintf.c
 * \brief Minimal MISRA-friendly printf replacement using fixed parameter counts.
 */

#include "mPrintf.h"
#include <string.h> /* copy of bytes to float */

#include "mString.h"

static pf_printf_output_t g_m_printf_output_fn = (pf_printf_output_t)0;
static void mPrintf_fillParams(uint64_t params[], uint64_t i_p1, uint64_t i_p2,
                               uint64_t i_p3, uint64_t i_p4, uint64_t i_p5,
                               uint64_t i_p6, uint64_t i_p7, uint64_t i_p8,
                               uint64_t i_p9, uint64_t i_p10);
static void mPrintf_streamOut(const char* outBuffer, uint32_t length);
static uint32_t mPrintf_cpToDest(char* dest, uint32_t destsz, const char* src,
                                 uint32_t i_srcLen);
typedef void (*m_printf_spec_handler_fn_t)(const uint64_t params[],
                                           uint32_t* argIndex, char* outBuffer,
                                           uint32_t outBufferSize,
                                           uint32_t* outLength, bool zero_pad,
                                           uint32_t min_width);

static uintptr_t mPrintf_u64ToUintptr(uint64_t value)
{
    uintptr_t result = (uintptr_t)0;
    (void)memcpy((void*)&result, (const void*)&value, sizeof(result));
    return result;
}
static const char* mPrintf_u64ToConstChar(uint64_t value)
{
    const char* result = (const char*)0;
    (void)memcpy((void*)&result, (const void*)&value, sizeof(result));
    return result;
}
static void mPrintf_appendChar(char* dest, uint32_t destsz, char i_char,
                               uint32_t* o_len)
{
    if ( (dest != NULL) && (o_len != NULL) && (destsz > 0U) ) {
        const uint32_t new_len = *o_len + 1U;
        if ( new_len < destsz ) {
            dest[*o_len] = i_char;
            *o_len       = new_len;
            dest[*o_len] = '\0';
        }
    }
}
static void mPrintf_appendCstring(char* dest, uint32_t destsz, const char* src,
                                  uint32_t* o_len)
{
    if ( (dest != NULL) && (src != NULL) && (o_len != NULL) && (destsz > 0U) ) {
        *o_len = m_strcat_s(dest, destsz, src);
        if ( *o_len < destsz ) {
            dest[*o_len] = '\0';
        }
    }
}
static void mPrintf_formatU32(char* dest, uint32_t destsz, uint32_t value,
                              uint32_t* o_len)
{
    char tmp[12];
    uint32_t converted;

    converted = m_utoa_s(value, tmp, (uint32_t)sizeof(tmp));
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatU32Pad(char* dest, uint32_t destsz, uint32_t value,
                                 uint32_t min_width, bool zero_pad,
                                 uint32_t* o_len)
{
    char tmp[12];
    uint32_t converted;

    converted = m_utoa_s_pad(value, tmp, (uint32_t)sizeof(tmp), min_width,
                             zero_pad);
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatS32(char* dest, uint32_t destsz, int32_t value,
                              uint32_t* o_len)
{
    char tmp[12];
    uint32_t converted;

    converted = m_itoa_s(value, tmp, (uint32_t)sizeof(tmp));
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatS32Pad(char* dest, uint32_t destsz, int32_t value,
                                 uint32_t min_width, bool zero_pad,
                                 uint32_t* o_len)
{
    char tmp[12];
    uint32_t converted;

    converted = m_itoa_s_pad(value, tmp, (uint32_t)sizeof(tmp), min_width,
                             zero_pad);
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatFloat64(char* dest, uint32_t destsz, float64_t value,
                                  uint32_t* o_len)
{
    char tmp[mString_FLOAT64_LEN];
    uint32_t converted;

    converted = m_ftoa_s(value, tmp, (uint32_t)sizeof(tmp), 6U);
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatPointerPrefix(char* dest, uint32_t destsz,
                                        uint32_t* o_len)
{
    mPrintf_appendChar(dest, destsz, '0', o_len);
    mPrintf_appendChar(dest, destsz, 'x', o_len);
}
static void mPrintf_formatPointerDigits(char* dest, uint32_t destsz,
                                        uintptr_t value, uint32_t* o_len)
{
    enum {
        NIBBLES = (sizeof(uintptr_t) * 2U)
    };
    char tmp[NIBBLES + 1];
    uint32_t converted;

    converted = m_uintptrHex_s(value, tmp, (uint32_t)sizeof(tmp), 1U);
    if ( converted > 0U ) {
        mPrintf_appendCstring(dest, destsz, tmp, o_len);
    }
}
static void mPrintf_formatPointer(char* dest, uint32_t destsz, uintptr_t value,
                                  uint32_t* o_len)
{
    if ( (dest != NULL) && (o_len != NULL) && (destsz != 0U) ) {
        mPrintf_formatPointerPrefix(dest, destsz, o_len);
        mPrintf_formatPointerDigits(dest, destsz, value, o_len);
    }
}
static float64_t mPrintf_u64ToFloat64(uint64_t i_bits)
{
    float64_t result;
    uint8_t bytes[sizeof(result)];

    for ( uint32_t i = 0U; i < sizeof(result); i++ ) {
        bytes[i] = (uint8_t)((i_bits >> (i * 8U)) & 0xFFU);
    }
    /*lint -e{857} this is deliberately abusing memcpy to reinterpret bits */
    (void)memcpy(&result, bytes, sizeof(result));
    return result;
}
static void mPrintf_addLiteral(char ch, char* outBuffer, uint32_t outBufferSize,
                               uint32_t* outLength)
{
    mPrintf_appendChar(outBuffer, outBufferSize, ch, outLength);
}
static void mPrintf_addPercent(char* outBuffer, uint32_t outBufferSize,
                               uint32_t* outLength)
{
    mPrintf_appendChar(outBuffer, outBufferSize, '%', outLength);
}
static void mPrintf_handleC(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    (void)zero_pad;
    (void)min_width;
    mPrintf_appendChar(outBuffer, outBufferSize, (char)params[*argIndex],
                       outLength);
    *argIndex = *argIndex + 1U;
}
static void mPrintf_handleS(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    const char* source = mPrintf_u64ToConstChar(params[*argIndex]);
    mPrintf_appendCstring(outBuffer, outBufferSize, source, outLength);
    *argIndex = *argIndex + 1U;
    (void)zero_pad;
    (void)min_width;
}
static void mPrintf_handleD(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    if ( zero_pad ) {
        mPrintf_formatS32Pad(outBuffer, outBufferSize,
                             (int32_t)params[*argIndex], min_width, zero_pad,
                             outLength);
    } else {
        mPrintf_formatS32(outBuffer, outBufferSize, (int32_t)params[*argIndex],
                          outLength);
    }
    *argIndex = *argIndex + 1U;
}
static void mPrintf_handleU(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    if ( zero_pad ) {
        mPrintf_formatU32Pad(outBuffer, outBufferSize,
                             (uint32_t)params[*argIndex], min_width, zero_pad,
                             outLength);
    } else {
        mPrintf_formatU32(outBuffer, outBufferSize, (uint32_t)params[*argIndex],
                          outLength);
    }
    *argIndex = *argIndex + 1U;
}
static void mPrintf_handleX(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    char tmp[12];
    uint32_t converted;

    converted = m_utoaHex_s((uint32_t)params[*argIndex], tmp,
                            (uint32_t)sizeof(tmp),
                            (zero_pad && (min_width > 1U)) ? min_width : 1U);
    if ( converted > 0U ) {
        mPrintf_appendCstring(outBuffer, outBufferSize, tmp, outLength);
    }
    *argIndex = *argIndex + 1U;
}
static void mPrintf_handleP(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    (void)zero_pad;
    (void)min_width;
    uintptr_t value = mPrintf_u64ToUintptr(params[*argIndex]);
    mPrintf_formatPointer(outBuffer, outBufferSize, value, outLength);
    *argIndex = *argIndex + 1U;
}
static void mPrintf_handleG(const uint64_t params[], uint32_t* argIndex,
                            char* outBuffer, uint32_t outBufferSize,
                            uint32_t* outLength, bool zero_pad,
                            uint32_t min_width)
{
    (void)zero_pad;
    (void)min_width;
    mPrintf_formatFloat64(outBuffer, outBufferSize,
                          mPrintf_u64ToFloat64(params[*argIndex]), outLength);
    *argIndex = *argIndex + 1U;
}
static uint32_t mPrintf_handleSpecBasic(char spec, const uint64_t params[],
                                        uint32_t* argIndex, char* outBuffer,
                                        uint32_t outBufferSize,
                                        uint32_t* outLength, bool zero_pad,
                                        uint32_t min_width)
{
    uint32_t handled = 0U;

    struct {
        char c;
        m_printf_spec_handler_fn_t fn;
    } handlers[] = {
        {'c', mPrintf_handleC},
        {'s', mPrintf_handleS},
        {'d', mPrintf_handleD},
        {'u', mPrintf_handleU},
    };

    for ( uint32_t i = 0U;
          i < (uint32_t)(sizeof(handlers) / sizeof(handlers[0])); i++ ) {
        if ( handlers[i].c == spec ) {
            handlers[i].fn(params, argIndex, outBuffer, outBufferSize,
                           outLength, zero_pad, min_width);
            handled = 1U;
            break;
        }
    }

    return handled;
}
static uint32_t mPrintf_handleSpecExtended(char spec, const uint64_t params[],
                                           uint32_t* argIndex, char* outBuffer,
                                           uint32_t outBufferSize,
                                           uint32_t* outLength, bool zero_pad,
                                           uint32_t min_width)
{
    uint32_t handled = 0U;

    struct {
        char c;
        m_printf_spec_handler_fn_t fn;
    } handlers[] = {
        {'x', mPrintf_handleX},
        {'p', mPrintf_handleP},
        {'g', mPrintf_handleG},
    };

    for ( uint32_t i = 0U;
          i < (uint32_t)(sizeof(handlers) / sizeof(handlers[0])); i++ ) {
        if ( handlers[i].c == spec ) {
            handlers[i].fn(params, argIndex, outBuffer, outBufferSize,
                           outLength, zero_pad, min_width);
            handled = 1U;
            break;
        }
    }

    return handled;
}
static void mPrintf_handleSpecifier(char spec, const uint64_t params[],
                                    uint32_t i_parameter_count,
                                    uint32_t* argIndex, char* outBuffer,
                                    uint32_t outBufferSize, uint32_t* outLength,
                                    bool zero_pad, uint32_t min_width)
{
    uint32_t handled = 0U;

    if ( *argIndex < i_parameter_count ) {
        handled = mPrintf_handleSpecBasic(spec, params, argIndex, outBuffer,
                                          outBufferSize, outLength, zero_pad,
                                          min_width);

        if ( handled == 0U ) {
            (void)mPrintf_handleSpecExtended(spec, params, argIndex, outBuffer,
                                             outBufferSize, outLength, zero_pad,
                                             min_width);
        }
    }
}
static uint32_t mPrintf_formatToBuffer(char* outBuffer, uint32_t outBufferSize,
                                       const char* i_format,
                                       uint32_t i_parameter_count,
                                       const uint64_t params[])
{
    uint32_t argIndex  = 0U;
    uint32_t outLength = 0U;
    uint32_t pos;
    uint32_t result = 0U;

    if ( (i_format == NULL) || (outBuffer == NULL) || (outBufferSize == 0U) ) {
        return result;
    }

    outBuffer[0] = '\0';
    outLength    = 0U;
    pos          = 0U;
    while ( i_format[pos] != '\0' ) {
        char ch = i_format[pos];

        if ( ch != '%' ) {
            mPrintf_addLiteral(ch, outBuffer, outBufferSize, &outLength);
            pos = pos + 1U;
            continue;
        }

        pos = pos + 1U;
        if ( i_format[pos] == '\0' ) {
            break;
        }

        if ( i_format[pos] == '%' ) {
            mPrintf_addPercent(outBuffer, outBufferSize, &outLength);
            pos = pos + 1U;
            continue;
        }

        bool zeroPad      = false;
        uint32_t minWidth = 0U;

        const uint8_t zeroChar = (uint8_t)'0';
        const uint8_t nineChar = (uint8_t)'9';
        if ( (uint8_t)i_format[pos] == zeroChar ) {
            zeroPad = true;
            pos     = pos + 1U;
        }
        uint8_t digitChar = (uint8_t)i_format[pos];
        while ( (digitChar >= zeroChar) && (digitChar <= nineChar) ) {
            minWidth = (minWidth * 10U)
                       + ((uint32_t)digitChar - (uint32_t)zeroChar);
            pos       = pos + 1U;
            digitChar = (uint8_t)i_format[pos];
        }

        if ( i_format[pos] == 'l' ) {
            pos = pos + 1U;
            if ( i_format[pos] == 'd' ) {
                if ( argIndex < i_parameter_count ) {
                    if ( zeroPad ) {
                        mPrintf_formatS32Pad(outBuffer, outBufferSize,
                                             (int32_t)params[argIndex],
                                             minWidth, zeroPad, &outLength);
                    } else {
                        mPrintf_formatS32(outBuffer, outBufferSize,
                                          (int32_t)params[argIndex],
                                          &outLength);
                    }
                    argIndex = argIndex + 1U;
                }
                pos = pos + 1U;
                continue;
            }

            mPrintf_addLiteral('%', outBuffer, outBufferSize, &outLength);
            mPrintf_addLiteral('l', outBuffer, outBufferSize, &outLength);
            continue;
        }

        /* #lizard forgives switch */
        mPrintf_handleSpecifier(i_format[pos], params, i_parameter_count,
                                &argIndex, outBuffer, outBufferSize, &outLength,
                                zeroPad, minWidth);
        pos = pos + 1U;
    }

    result = outLength;
    return result;
}
uint32_t mPrintf_private(const char* i_format, uint32_t i_parameter_count,
                         uint64_t i_p1, uint64_t i_p2, uint64_t i_p3,
                         uint64_t i_p4, uint64_t i_p5, uint64_t i_p6,
                         uint64_t i_p7, uint64_t i_p8, uint64_t i_p9,
                         uint64_t i_p10)
{
    uint64_t params[M_PRINTF_MAX_PARAMETERS];
    char outBuffer[M_PRINTF_BUFFER_SIZE];
    uint32_t length;
    uint32_t result = 0U;

    if ( i_format == NULL ) {
        return result;
    }
    mPrintf_fillParams(params, i_p1, i_p2, i_p3, i_p4, i_p5, i_p6, i_p7, i_p8,
                       i_p9, i_p10);
    length = mPrintf_formatToBuffer(outBuffer, M_PRINTF_BUFFER_SIZE, i_format,
                                    i_parameter_count, params);
    mPrintf_streamOut(outBuffer, length);

    result = length;
    return result;
}
static void mPrintf_fillParams(uint64_t params[], uint64_t i_p1, uint64_t i_p2,
                               uint64_t i_p3, uint64_t i_p4, uint64_t i_p5,
                               uint64_t i_p6, uint64_t i_p7, uint64_t i_p8,
                               uint64_t i_p9, uint64_t i_p10)
{
    /* clang-format off */
    params[0] = i_p1;  params[1] = i_p2;  params[2] = i_p3;  params[3] = i_p4;
    params[4] = i_p5;  params[5] = i_p6;  params[6] = i_p7;  params[7] = i_p8;
    params[8] = i_p9;  params[9] = i_p10;
    /* clang-format on */
}
static void mPrintf_streamOut(const char* outBuffer, uint32_t length)
{
    if ( g_m_printf_output_fn != (pf_printf_output_t)0 ) {
        for ( uint32_t ui = 0U; ui < length; ui++ ) {
            g_m_printf_output_fn(outBuffer[ui]);
        }
    }
}
uint32_t mSnprintf_general(char* dest, uint32_t destsz, const char* i_format,
                           uint32_t i_parameter_count, uint64_t i_p1,
                           uint64_t i_p2, uint64_t i_p3, uint64_t i_p4,
                           uint64_t i_p5, uint64_t i_p6, uint64_t i_p7,
                           uint64_t i_p8, uint64_t i_p9, uint64_t i_p10)
{
    uint64_t params[M_PRINTF_MAX_PARAMETERS];
    char buffer[M_PRINTF_BUFFER_SIZE];
    uint32_t formatted;
    uint32_t copyCount;
    uint32_t result = 0U;

    if ( (dest == NULL) || (destsz == 0U) ) {
        return result;
    }

    mPrintf_fillParams(params, i_p1, i_p2, i_p3, i_p4, i_p5, i_p6, i_p7, i_p8,
                       i_p9, i_p10);

    formatted = mPrintf_formatToBuffer(buffer, M_PRINTF_BUFFER_SIZE, i_format,
                                       i_parameter_count, params);

    copyCount = mPrintf_cpToDest(dest, destsz, buffer, formatted);

    result = copyCount;
    return result;
}
static uint32_t mPrintf_cpToDest(char* dest, uint32_t destsz, const char* src,
                                 uint32_t i_srcLen)
{
    uint32_t copyCount;

    copyCount = 0U;
    while ( copyCount < i_srcLen ) {
        const uint32_t nextPos = copyCount + 1U;
        if ( nextPos >= destsz ) {
            break;
        }

        dest[copyCount] = src[copyCount];
        copyCount       = nextPos;
    }

    dest[copyCount] = '\0';
    return copyCount;
}
void m_printf_setOutput(pf_printf_output_t i_output_fn)
{
    g_m_printf_output_fn = i_output_fn;
}
