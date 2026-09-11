#include <limits.h>
#include <stddef.h>

#include "libAscii.h"
#include "mStrToNum.h"


static bool mStrToNum_toDigit(const char_t ch, uint8_t* o_digit)
{
    bool valid = false;
    if ( (ch >= (char_t)'0') && (ch <= (char_t)'9') ) {
        uint8_t digit = (uint8_t)((uint8_t)ch - (uint8_t)'0');
        *o_digit      = digit;
        valid         = true;
    }
    return valid;
}
static bool mStrToNum_boundsCheckS32(int64_t value, bool negative)
{
    bool valid = true;
    if ( negative ) {
        if ( value > ((int64_t)INT32_MAX + 1LL) ) {
            valid = false;
        }
    } else {
        if ( value > INT32_MAX ) {
            valid = false;
        }
    }
    return valid;
}
static bool mStrToNum_boundsCheckU32(uint64_t value)
{
    return (value <= UINT32_MAX);
}
static bool mStrToNum_boundsCheckU16(uint32_t value)
{
    return (value <= (uint32_t)(UINT16_MAX));
}
static bool mStrToNum_boundsCheckU8(uint32_t value)
{
    return (value <= (uint32_t)(UINT8_MAX));
}
static bool mStrToNum_charNegative(const char_t* ch, uint8_t* index)
{
    bool negative = false;
    if ( ch[*index] == (char_t)('-') ) {
        *index   = *index + 1U;
        negative = true;
    }
    return negative;
}
static bool mStrToNum_scanU32(const char_t* str, uint8_t length,
                              uint32_t* o_value)
{
    uint64_t result = 0ULL;
    uint8_t ix      = 0U;
    bool valid      = false;

    while ( (ix < length) && (str[ix] != (char_t)('\0')) ) {
        char_t ch = str[ix];
        uint8_t digit;
        valid = mStrToNum_toDigit(ch, &digit);
        if ( valid ) {
            uint64_t next = (result * 10ULL) + (uint64_t)digit;
            valid         = mStrToNum_boundsCheckU32(next);
            if ( valid ) {
                result = next;
            }
        }
        if ( !valid ) {
            break;
        }
        ix++;

        if ( valid ) {
            *o_value = (uint32_t)result;
        }
    }

    return valid;
}
/*******************************************************************************
 *                      Exposed Interfaces
 *****************************************************************************/
bool matoi_u32(const char_t* str, uint8_t length, uint32_t* value)
{
    bool valid = ((str != NULL) && (value != NULL) && (length > 0U));
    *value     = 0U;

    if ( valid ) {
        valid = mStrToNum_scanU32(str, length, value);
    }

    return valid;
}

bool matoi_s32(const char_t* str, uint8_t length, int32_t* value)
{
    bool valid     = ((str != NULL) && (value != NULL) && (length > 0U));
    int64_t result = 0;
    bool negative  = false;

    if ( valid ) {
        uint8_t ix         = 0U;
        uint32_t u32_value = 0U;

        negative = mStrToNum_charNegative(str, &ix);

        valid  = mStrToNum_scanU32((const char_t*)(str + ix),
                                   (uint8_t)(length - ix), &u32_value);
        result = (int64_t)u32_value;
    }
    if ( valid ) {
        valid = mStrToNum_boundsCheckS32(result, negative);
    }
    if ( valid ) {
        *value = negative ? (int32_t)(-result) : (int32_t)result;
    }


    return valid;
}

bool matoi_u16(const char_t* str, uint8_t length, uint16_t* value)
{
    *value          = 0U;
    uint32_t result = 0U;

    bool valid = matoi_u32(str, length, &result);
    if ( valid ) {
        valid = mStrToNum_boundsCheckU16(result);
    }

    if ( valid ) {
        *value = (uint16_t)result;
    }


    return valid;
}

bool matoi_u8(const char_t* str, uint8_t length, uint8_t* value)
{
    *value          = 0U;
    uint32_t result = 0U;

    bool valid = matoi_u32(str, length, &result);
    if ( valid ) {
        valid = mStrToNum_boundsCheckU8(result);
    }
    if ( valid ) {
        *value = (uint8_t)result;
    }

    return valid;
}
