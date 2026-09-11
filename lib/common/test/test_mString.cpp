
#include <gtest/gtest.h>
#include <iomanip>

#include "mPrintf.h"
#include "mString.h"
#include "utilDevDebug.h"

using namespace testing;

/* clang-format off */
static const float64_t tLut[] = {
    /* zero */
    0,
    /* smallest round up */
    -999999999.999999999,
    /* biggest round up */
    999999999.999999999,
    /* travelling 1, negative number */
    -100000000.0000000000,
    -010000000.0000000000,
    -001000000.0000000000,
    -000100000.0000000000,
    -000010000.0000000000,
    -000001000.0000000000,
    -000000100.0000000000,
    -000000010.0000000000,
    -000000001.0000000000,
    -000000000.1000000000,
    -000000000.0100000000,
    -000000000.0010000000,
    -000000000.0001000000,
    -000000000.0000100000,
    -000000000.0000010000,
    -000000000.0000001000,
    -000000000.0000000100,
    -000000000.0000000010,
    -000000000.0000000001,
    /* 9 */
    -900000000.0000000000,
    -090000000.0000000000,
    -009000000.0000000000,
    -000900000.0000000000,
    -000090000.0000000000,
    -000009000.0000000000,
    -000000900.0000000000,
    -000000090.0000000000,
    -000000009.0000000000,
    -000000000.9000000000,
    -000000000.0900000000,
    -000000000.0090000000,
    -000000000.0009000000,
    -000000000.0000900000,
    -000000000.0000090000,
    -000000000.0000009000,
    -000000000.0000000900,
    -000000000.0000000090,
    -000000000.0000000009,

    /* travelling 1, positive number */
    100000000.0000000000,
    010000000.0000000000,
    001000000.0000000000,
    000100000.0000000000,
    000010000.0000000000,
    000001000.0000000000,
    000000100.0000000000,
    000000010.0000000000,
    000000001.0000000000,
    000000000.1000000000,
    000000000.0100000000,
    000000000.0010000000,
    000000000.0001000000,
    000000000.0000100000,
    000000000.0000010000,
    000000000.0000001000,
    000000000.0000000100,
    000000000.0000000010,
    000000000.0000000001,
    /* 9 */
    900000000.0000000000,
    090000000.0000000000,
    009000000.0000000000,
    000900000.0000000000,
    000090000.0000000000,
    000009000.0000000000,
    000000900.0000000000,
    000000090.0000000000,
    000000009.0000000000,
    000000000.9000000000,
    000000000.0900000000,
    000000000.0090000000,
    000000000.0009000000,
    000000000.0000900000,
    000000000.0000090000,
    000000000.0000009000,
    000000000.0000000900,
    000000000.0000000090,
    000000000.0000000009,

    /* all digits positive */
    1.023456789,
    9.0234567891,
    /* all digits negative */
    -1.023456789,
    -9.0234567891,
    /* misc */
    -99.99,
    1.23,
    -1.23,
    1.1e1,
    1.234e7,
    1.234e9,

};
/* clang-format on */


/* m_strlen_s */


TEST(strlen, Ret0NullPtr)
{
    EXPECT_TRUE(0 == m_strlen_s(0, 123));
}

TEST(strlen, Ret0NoNullTerm)
{
    const uint8_t badStr[] = "123";

    EXPECT_TRUE(0 == m_strlen_s((const char*)(badStr), 2));
    (void)(badStr);
}

TEST(strlen, Ret01stCharNullTerm)
{
    const uint8_t badStr[] = "0123";

    EXPECT_TRUE(0 == m_strlen_s((const char*)(badStr), 2));
    (void)(badStr);
}

TEST(strlen, Ret0NullTermOutBounds)
{
    EXPECT_TRUE(0 == m_strlen_s("Hello world!", 5));
}

TEST(strlen, Ret12HelloWorld)
{
    EXPECT_TRUE(12 == m_strlen_s("Hello world!", 123));
}

/* m_strcat_s */

TEST(strcat, Ret0NullPtrOnSrc)
{
    const char unused[] = "unused";

    EXPECT_TRUE(0 == m_strcat_s(0, 1, unused));
}

TEST(strcat, Ret0NullPtrDest)
{
    char unused[] = "unused";

    EXPECT_TRUE(0 == m_strcat_s(unused, 1, 0));
}

TEST(strcat, Ret0ZeroDestSize)
{
    const char unused[] = "unused";
    char foo[]          = "foo";

    EXPECT_TRUE(0 == m_strcat_s(foo, 0, unused));
}

TEST(strcat, Append2emptySrcNdoesNotFit)
{
    const char src[] = "foo";
    char dest[2]     = "\0";

    EXPECT_TRUE(0 == m_strcat_s(dest, 2, src));
    EXPECT_TRUE(0 == strcmp("\0", dest));
}

TEST(strcat, Append2emptySrc)
{
    const char src[] = "foo";
    char dest[4];
    uint32_t destLen = sizeof(dest);
    dest[0]          = '\0';
    EXPECT_TRUE(3 == m_strcat_s(dest, destLen, src));
    EXPECT_TRUE(0 == strcmp("foo", dest));
}

TEST(strcat, Append2nonEmptySrc)
{
    const char src[] = "foo";
    char dest[]      = {'b', 'a', 'r', '\0', '1', '2', '3'};
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(6 == m_strcat_s(dest, destLen, src));
    EXPECT_TRUE(0 == strcmp("barfoo", dest));
}


/* m_itoa_s */

TEST(itoa, Ret0NullPtrDest)
{
    EXPECT_TRUE(0 == m_itoa_s(0, 0, 123));
}

TEST(itoa, Ret0bufSz0)
{
    char unused[] = "unused";
    EXPECT_TRUE(0 == m_itoa_s(0, unused, 0));
}

TEST(itoa, Ret0buf2small)
{
    char unused[1];
    uint32_t destLen = sizeof(unused);

    EXPECT_TRUE(0 == m_itoa_s(0, unused, destLen));
}

TEST(itoa, ConvNums)
{
    char dest[12];
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(1 == m_itoa_s(0, dest, destLen));
    EXPECT_TRUE(0 == strcmp("0", dest));

    EXPECT_TRUE(4 == m_itoa_s(-723, dest, destLen));
    EXPECT_TRUE(0 == strcmp("-723", dest));

    EXPECT_TRUE(3 == m_itoa_s(123, dest, destLen));
    EXPECT_TRUE(0 == strcmp("123", dest));

    EXPECT_TRUE(11 == m_itoa_s(-1023456789, dest, destLen));
    EXPECT_TRUE(0 == strcmp("-1023456789", dest));

    EXPECT_TRUE(10 == m_itoa_s(1023456789, dest, destLen));
    EXPECT_TRUE(0 == strcmp("1023456789", dest));

    EXPECT_TRUE(11 == m_itoa_s(-2147483648, dest, destLen));
    EXPECT_TRUE(0 == strcmp("-2147483648", dest));

    EXPECT_TRUE(10 == m_itoa_s(2147483647, dest, destLen));
    EXPECT_TRUE(0 == strcmp("2147483647", dest));
}

TEST(itoa, Ret0WhenDestTooSmallForPadding)
{
    char dest[10]    = "";
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(0 == m_itoa_s_pad(-1, dest, destLen, 10U, true));
}


/* m_utoa_s */

TEST(utoa, Ret0NullPtrDest)
{
    EXPECT_TRUE(0 == m_utoa_s(0, 0, 123));
}

TEST(utoa, Ret0bufSz0)
{
    char unused[] = "unused";
    EXPECT_TRUE(0 == m_utoa_s(0, unused, 0));
}

TEST(utoa, Ret0buf2small)
{
    char unused[1];
    uint32_t destLen = sizeof(unused);

    EXPECT_TRUE(0 == m_utoa_s(0, unused, destLen));
}

TEST(utoa, ConvNums)
{
    char dest[mString_UINT32_LEN];
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(1 == m_utoa_s(0, dest, destLen));
    EXPECT_TRUE(0 == strcmp("0", dest));

    EXPECT_TRUE(10 == m_utoa_s(1023456789, dest, destLen));
    EXPECT_TRUE(0 == strcmp("1023456789", dest));

    EXPECT_TRUE(10 == m_utoa_s(4294967295, dest, destLen));
    EXPECT_TRUE(0 == strcmp("4294967295", dest));
}

TEST(utoa, Ret0WhenDestTooSmallForPadding)
{
    char dest[10]    = "";
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(0 == m_utoa_s_pad(1U, dest, destLen, 10U, true));
}


TEST(m_printf, SnprintfBasic)
{
    char out[64];
    uint32_t len = m_snprintf_5(out, sizeof(out), "Hello %s %d %u %x %c %%",
                                "world", -42, 42U, 0x2aU, 'Z');

    EXPECT_TRUE(0 == strcmp(out, "Hello world -42 42 2a Z %"));
    EXPECT_TRUE(len == 25U);
}

TEST(m_printf, SnprintfOverflow)
{
    char out[8];
    uint32_t len = m_snprintf_0(out, sizeof(out), "ABCDEFGHJKLMN");

    EXPECT_TRUE(0 == strcmp(out, "ABCDEFG"));
    EXPECT_TRUE(len == 7U);
}

static char g_mprintf_capture[256];
static uint32_t g_mprintf_capture_len;

static void m_printf_capture(char c)
{
    if ( g_mprintf_capture_len < (sizeof(g_mprintf_capture) - 1U) ) {
        g_mprintf_capture[g_mprintf_capture_len++] = c;
        g_mprintf_capture[g_mprintf_capture_len]   = '\0';
    }
}

TEST(m_printf, PrintfCallback)
{
    g_mprintf_capture_len = 0U;
    g_mprintf_capture[0]  = '\0';

    m_printf_setOutput(m_printf_capture);
    m_printf_3("X %u %s %c%%", 123U, "abc", 'Q');

    EXPECT_TRUE(0 == strcmp(g_mprintf_capture, "X 123 abc Q%"));
    EXPECT_TRUE(12U == g_mprintf_capture_len);

    m_printf_setOutput(NULL);
}


void test_m_ftoa_helper(uint32_t pr, const char* str_pr)
{
    char str[23];
    char libstr[23];
    uint32_t strLen = sizeof(str);
    uint32_t u32Ix  = 0;

    do {
        /* keep for debug DD_TRACE("\ntest-case pr=%d [%d] f=%f\n", pr, u32Ix, tLut[u32Ix]);*/
        uint32_t len    = m_ftoa_s(tLut[u32Ix], str, strLen, pr);
        uint32_t liblen = snprintf(&libstr[0], 23, str_pr, tLut[u32Ix]);
        /* keep for debug DD_TRACE("##########result [%s]\n", str);
        DD_TRACE(">>>>>>>>snprintf [%s]\n", libstr);*/
        EXPECT_TRUE(0 == strcmp(str, libstr));
        EXPECT_TRUE(len == liblen);

        u32Ix++;
    } while ( u32Ix < (sizeof(tLut) / sizeof(float64_t)) );
}

/* m_ftoa_s */

TEST(ftoa, Ret0NullPtrDest)
{
    EXPECT_TRUE(0 == m_ftoa_s(0, 0, 23, 10));
}

TEST(ftoa, Ret0bufSz0)
{
    char unused[] = "unused";
    EXPECT_TRUE(0 == m_ftoa_s(0, unused, 0, 10));
}

TEST(ftoa, Ret0buf2small)
{
    char unused[1];
    uint32_t destLen = sizeof(unused);

    EXPECT_TRUE(0 == m_ftoa_s(0, unused, destLen, 10));
}

TEST(ftoa, ConvIfPrecGtr0)
{
    char str[23];
    uint32_t strLen = sizeof(str);
    char libstr[23];

    uint32_t len    = m_ftoa_s(1.23, str, strLen, 11);
    uint32_t liblen = snprintf(&libstr[0], 23, "%.10f", 1.23);
    /* keep for debug DD_TRACE("##########result [%s]\n", str);
    DD_TRACE(">>>>>>>>snprintf [%s]\n", libstr);*/
    EXPECT_TRUE(0 == strcmp(str, libstr));
    EXPECT_TRUE(len == liblen);
}


TEST(ftoa, MaxPrec10f)
{
    test_m_ftoa_helper(10U, "%.10f");
}
TEST(ftoa, MinPrec1f)
{
    test_m_ftoa_helper(1U, "%.1f");
}

TEST(ftoa, MinPrec6f)
{
    test_m_ftoa_helper(6U, "%.6f");
}

/* m_strstr_s */
TEST(strstr, BadParams)
{
    const char* foo = "foo";
    EXPECT_TRUE(NULL == m_strstr_s(NULL, 10, foo, 10));
    EXPECT_TRUE(NULL == m_strstr_s(foo, 0, foo, 10));
    EXPECT_TRUE(NULL == m_strstr_s(foo, 10, NULL, 10));
    EXPECT_TRUE(NULL == m_strstr_s(foo, 10, foo, 0));
    EXPECT_TRUE(NULL == m_strstr_s("\0", 10, foo, 10));
    EXPECT_TRUE(NULL == m_strstr_s(foo, 10, "\0", 10));
}
TEST(strstr, EqualStrs)
{
    const char* foo = "foo";
    uint32_t uiLen  = m_strlen_s(foo, 5);

    EXPECT_TRUE(foo == m_strstr_s(foo, uiLen, foo, uiLen));
}

TEST(strstr, ArinBar)
{
    const char* bar    = "bar";
    const char* result = bar;
    result++;
    EXPECT_TRUE(result == m_strstr_s(bar, 3, "ar", 2));
}

TEST(strstr, IsFound)
{
    const char* pStr  = "set_field_number 123 12.3";
    uint32_t uiStrlen = m_strlen_s(pStr, 30);
    const char* pSub  = "set_field_number";
    uint32_t uiSublen = m_strlen_s(pSub, 30);
    char* result      = (char*)(pStr);
    EXPECT_TRUE(result == m_strstr_s(pStr, uiStrlen, pSub, uiSublen));
    result += uiSublen + 1;
    EXPECT_TRUE(result == m_strstr_s(pStr, uiStrlen, "123", 3));
    result += 3 + 2;
    EXPECT_TRUE(result == m_strstr_s(pStr, uiStrlen, "2.3", 3));
}

TEST(strstr, IsNotFound)
{
    const char* pStr  = "set_field_number 123 12.3";
    uint32_t uiStrlen = m_strlen_s(pStr, 30);

    EXPECT_TRUE(NULL == m_strstr_s(pStr, uiStrlen, "1.2", 3));
}

TEST(strcat, DestSize1)
{
    char dest[1]     = "";
    const char src[] = "a";
    EXPECT_TRUE(0 == m_strcat_s(dest, 1, src));
}


TEST(strcat, SrcNotNullTerminated)
{
    char dest[8] = "foo";
    char src[2]  = {'b', 'c'}; // no null terminator
    EXPECT_TRUE(0 == m_strcat_s(dest, 8, src));
}

TEST(ftoa, PrecisionGreaterThan10)
{
    char str[32];
    uint32_t len = m_ftoa_s(3.1415926535, str, sizeof(str), 20);
    char ref[32];
    uint32_t reflen = snprintf(ref, sizeof(ref), "%.10f", 3.1415926535);
    EXPECT_TRUE(0 == strcmp(str, ref));
    EXPECT_TRUE(len == reflen);
}

TEST(ftoa, BufferExactly23)
{
    char str[23];
    uint32_t len = m_ftoa_s(1.23456789, str, 23, 6);
    char ref[23];
    uint32_t reflen = snprintf(ref, sizeof(ref), "%.6f", 1.23456789);
    EXPECT_TRUE(0 == strcmp(str, ref));
    EXPECT_TRUE(len == reflen);
}

TEST(strstr, StrszLessThanSublen)
{
    const char* str = "abc";
    const char* sub = "abcd";
    EXPECT_TRUE(NULL == m_strstr_s(str, 3, sub, 4));
}

TEST(strstr, StrszZero)
{
    const char* str = "abc";
    const char* sub = "a";
    EXPECT_TRUE(NULL == m_strstr_s(str, 0, sub, 1));
}

TEST(strstr, SublenZero)
{
    const char* str = "abc";
    const char* sub = "a";
    EXPECT_TRUE(NULL == m_strstr_s(str, 3, sub, 0));
}

TEST(strstr, SubstringAtEnd)
{
    const char* str = "abcdef";
    const char* sub = "ef";
    EXPECT_TRUE(&str[4] == m_strstr_s(str, 6, sub, 2));
}

TEST(strstr, SubstringNotFound)
{
    const char* str = "abcdef";
    const char* sub = "gh";
    EXPECT_TRUE(NULL == m_strstr_s(str, 6, sub, 2));
}


/* m_strcmp_s */

TEST(strcmp, BothNullPtr)
{
    EXPECT_TRUE(1 == m_strcmp_s(NULL, NULL, 10));
}

TEST(strcmp, FirstNullPtr)
{
    const char* foo = "foo";
    EXPECT_TRUE(1 == m_strcmp_s(NULL, foo, 10));
}

TEST(strcmp, SecondNullPtr)
{
    const char* foo = "foo";
    EXPECT_TRUE(1 == m_strcmp_s(foo, NULL, 10));
}

TEST(strcmp, BothEmptyStrings)
{
    const char* empty1 = "";
    const char* empty2 = "";
    EXPECT_TRUE(0 == m_strcmp_s(empty1, empty2, 10));
}

TEST(strcmp, EqualStrings)
{
    const char* str1 = "hello";
    const char* str2 = "hello";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, SameStringToSelf)
{
    const char* foo = "foo";
    EXPECT_TRUE(0 == m_strcmp_s(foo, foo, 10));
}

TEST(strcmp, DifferentFirstChar)
{
    const char* str1 = "apple";
    const char* str2 = "bpple";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, DifferentMiddleChar)
{
    const char* str1 = "hello";
    const char* str2 = "hallo";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, DifferentLastChar)
{
    const char* str1 = "test";
    const char* str2 = "tesa";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, SamePrefixDifferentLength)
{
    const char* str1 = "test";
    const char* str2 = "testing";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, ZeroSize)
{
    const char* str1 = "hello";
    const char* str2 = "hello";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 0));
}

TEST(strcmp, SizeOneMatch)
{
    const char* str1 = "a";
    const char* str2 = "a";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 1));
}

TEST(strcmp, SizeOneMismatch)
{
    const char* str1 = "a";
    const char* str2 = "b";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 1));
}

TEST(strcmp, SizeLargerThanString)
{
    const char* str1 = "abc";
    const char* str2 = "abc";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 100));
}

TEST(strcmp, WithNumbers)
{
    const char* str1 = "test123";
    const char* str2 = "test123";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, NumbersMismatch)
{
    const char* str1 = "test123";
    const char* str2 = "test124";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, SpecialCharacters)
{
    const char* str1 = "hello@world.com";
    const char* str2 = "hello@world.com";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 20));
}

TEST(strcmp, CaseSensitive)
{
    const char* str1 = "Hello";
    const char* str2 = "hello";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 10));
}

TEST(strcmp, WhitespaceCharacters)
{
    const char* str1 = "hello world";
    const char* str2 = "hello world";
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 20));
}

TEST(strcmp, WhitespaceMismatch)
{
    const char* str1 = "hello world";
    const char* str2 = "helloworld";
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 20));
}

TEST(strcmp, PartialCompareWithinSize)
{
    const char* str1 = "abcdef";
    const char* str2 = "abcxyz";
    /* With i_strsz=3, the function compares 4 characters (indices 0-3).
     * Since 'd' != 'x' at index 3, it returns 1 (not equal) */
    EXPECT_TRUE(1 == m_strcmp_s(str1, str2, 3));
}

TEST(strcmp, PartialCompareWithinSizeMatch)
{
    const char* str1 = "abc";
    const char* str2 = "abc";
    /* With i_strsz=6 (string length + 1), null terminator is found at index 3,
     * returns 0 (equal) */
    EXPECT_TRUE(0 == m_strcmp_s(str1, str2, 6));
}

/* m_utoa8Hex_s*/
TEST(utoa8Hex, Ret0NullPtrDest)
{
    EXPECT_TRUE(0 == m_utoa8Hex_s(0, 0, 2));
}
TEST(utoa8Hex, Ret0bufSz0)
{
    char unused[] = "unused";
    EXPECT_TRUE(0 == m_utoa8Hex_s(0, unused, 0));
}
TEST(utoa8Hex, Ret0buf2small)
{
    char unused[1];
    uint32_t destLen = sizeof(unused);

    EXPECT_TRUE(0 == m_utoa8Hex_s(0, unused, destLen));
}
TEST(utoa8Hex, ConvNums)
{
    char dest[3];
    uint32_t destLen = sizeof(dest);

    EXPECT_TRUE(2 == m_utoa8Hex_s(0x00, dest, destLen));
    EXPECT_TRUE(0 == strcmp("00", dest));

    EXPECT_TRUE(2 == m_utoa8Hex_s(0x05, dest, destLen));
    EXPECT_TRUE(0 == strcmp("05", dest));

    EXPECT_TRUE(2 == m_utoa8Hex_s(0x0A, dest, destLen));
    EXPECT_TRUE(0 == strcmp("0A", dest));

    EXPECT_TRUE(2 == m_utoa8Hex_s(0xFF, dest, destLen));
    EXPECT_TRUE(0 == strcmp("FF", dest));
}
