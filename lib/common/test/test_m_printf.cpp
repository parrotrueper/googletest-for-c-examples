#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>

#include "mPrintf.h"

static char s_capture_buffer[256];
static uint32_t s_capture_len;

static void m_printf_capture(char c)
{
    if ( s_capture_len < (sizeof(s_capture_buffer) - 1U) ) {
        s_capture_buffer[s_capture_len++] = c;
        s_capture_buffer[s_capture_len]   = '\0';
    }
}

TEST(m_printf, CustomSnprintfBasic)
{
    char out[64];
    uint32_t len = m_snprintf_8(out, sizeof(out),
                                "Hello %s %d %u %x %c %% %p %ld %g", "world",
                                -42, 42U, 0x2aU, 'Z', (void*)0x1234U,
                                (long)1234, m_printf_doubleToU64(3.14));

    EXPECT_STREQ(out, "Hello world -42 42 2a Z % 0x1234 1234 3.140000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfWithNumPadding)
{
    char out[64];
    uint32_t len = m_snprintf_3(out, sizeof(out), "%05d %04u %02x", -42, 42U,
                                0x5U);

    EXPECT_STREQ(out, "-0042 0042 05");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfHexZeroValue)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%04x", 0U);

    EXPECT_STREQ(out, "0000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfPointerHexLetters)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%p", (void*)0xabcdefUL);

    EXPECT_STREQ(out, "0xabcdef");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfLongZeroPad)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%05ld", (long)-42);

    EXPECT_STREQ(out, "-0042");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfLongPositiveZeroPad)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%05ld", (long)42);

    EXPECT_STREQ(out, "00042");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfLongMinValue)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%05ld", (long)INT32_MIN);

    EXPECT_STREQ(out, "-2147483648");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfCharAndPercent)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "[%c]%%", 'A');

    EXPECT_STREQ(out, "[A]%");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfPointerNull)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%p", (void*)NULL);

    EXPECT_STREQ(out, "0x0");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfBufferExactFit)
{
    char out[5];
    uint32_t len = m_snprintf_0(out, sizeof(out), "1234");

    EXPECT_STREQ(out, "1234");
    EXPECT_EQ(len, 4U);
}

TEST(m_printf, snprintfEmptyFormat)
{
    char out[16] = "XYZ";
    uint32_t len = m_snprintf_0(out, sizeof(out), "");

    EXPECT_STREQ(out, "");
    EXPECT_EQ(len, 0U);
}

TEST(m_printf, snprintfMinWidthWithoutZeroPadIsIgnored)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%5u", 42U);

    EXPECT_STREQ(out, "42");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfHexMaxValue)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%08x", UINT32_MAX);

    EXPECT_STREQ(out, "ffffffff");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfFloatZero)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%g",
                                m_printf_doubleToU64(0.0));

    EXPECT_STREQ(out, "0.000000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfStringWithWidthNotSupported)
{
    char out[16];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%10s", "abc");

    EXPECT_STREQ(out, "abc");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfMissingParameterKeepsSpacing)
{
    char out[32];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%d %d %d", 1);

    EXPECT_STREQ(out, "1  ");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfNoArgFormat)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "plain text");

    EXPECT_STREQ(out, "plain text");
    EXPECT_EQ(len, 10U);
}

TEST(m_printf, snprintfUnknownSpecifierAfterPercent)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "x%qz");

    EXPECT_STREQ(out, "xz");
    EXPECT_EQ(len, 2U);
}

TEST(m_printf, snprintfUnsupportedLongModifier)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "x%lz");

    EXPECT_STREQ(out, "x%lz");
    EXPECT_EQ(len, 4U);
}

TEST(m_printf, snprintfNullDestPreservesNoWrite)
{
    char out[2]  = {'A', '\0'};
    uint32_t len = m_snprintf_1(out, 0U, "%u", 99U);

    EXPECT_EQ(len, 0U);
    EXPECT_EQ(out[0], 'A');
}

TEST(m_printf, snprintfPercentOnly)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "100%% done");

    EXPECT_STREQ(out, "100% done");
    EXPECT_EQ(len, 9U);
}

TEST(m_printf, snprintfFloatScientific)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%g",
                                m_printf_doubleToU64(1234567.0));

    EXPECT_STREQ(out, "1234567.000000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfMultipleSpecifiers)
{
    char out[64];
    uint32_t len = m_snprintf_5(out, sizeof(out), "%c %s %d %u %x", 'A', "B",
                                -1, 65535U, 255U);

    EXPECT_STREQ(out, "A B -1 65535 ff");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfPointerLarge)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%p", (void*)0x12345678ULL);

    EXPECT_STREQ(out, "0x12345678");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfLongNoZeroPad)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%ld", (long)123456789L);

    EXPECT_STREQ(out, "123456789");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfS32ZeroPadNoPadding)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%05d", 12345);

    EXPECT_STREQ(out, "12345");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfU32ZeroPadZeroValue)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%04u", 0U);

    EXPECT_STREQ(out, "0000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfEmptyStringParameter)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%s", "");

    EXPECT_STREQ(out, "");
    EXPECT_EQ(len, 0U);
}

TEST(m_printf, snprintfIntAndHexMaxValues)
{
    char out[64];
    uint32_t len = m_snprintf_3(out, sizeof(out), "%d %u %x", INT32_MAX,
                                UINT32_MAX, UINT32_MAX);

    EXPECT_STREQ(out, "2147483647 4294967295 ffffffff");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfTenParameters)
{
    char out[128];
    uint32_t len = m_snprintf_10(out, sizeof(out),
                                 "%u %u %u %u %u %u %u %u %u %u", 1U, 2U, 3U,
                                 4U, 5U, 6U, 7U, 8U, 9U, 10U);

    EXPECT_STREQ(out, "1 2 3 4 5 6 7 8 9 10");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfStringTruncates)
{
    char out[8];
    uint32_t len = m_snprintf_1(out, sizeof(out), "hello %s", "world");

    EXPECT_STREQ(out, "hello w");
    EXPECT_EQ(len, 7U);
}

TEST(m_printf, snprintfFloatNegative)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%g",
                                m_printf_doubleToU64(-3.141592653589793));

    EXPECT_STREQ(out, "-3.141593");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfS32ZeroPadZeroValue)
{
    char out[64];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%05d", 0);

    EXPECT_STREQ(out, "00000");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, snprintfFormatNull)
{
    char out[16] = "UNCHANGED";
    uint32_t len = mSnprintf_general(out, sizeof(out), NULL, 0U, 0ULL, 0ULL,
                                     0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,
                                     0ULL);

    EXPECT_EQ(len, 0U);
    EXPECT_STREQ(out, "");
}

TEST(m_printf, snprintfTrailingPercent)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "abc%");

    EXPECT_STREQ(out, "abc");
    EXPECT_EQ(len, 3U);
}

TEST(m_printf, snprintfInvalidLongSpecifier)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "x%lz");

    EXPECT_STREQ(out, "x%lz");
    EXPECT_EQ(len, 4U);
}

TEST(m_printf, snprintfZeroDestinationSize)
{
    char out[2]  = {'X', '\0'};
    uint32_t len = m_snprintf_1(out, 0U, "%u", 1U);

    EXPECT_EQ(len, 0U);
    EXPECT_EQ(out[0], 'X');
}

TEST(m_printf, snprintfNullStringParameter)
{
    char out[16];
    uint32_t len = m_snprintf_1(out, sizeof(out), "%s", (const char*)NULL);

    EXPECT_STREQ(out, "");
    EXPECT_EQ(len, 0U);
}

TEST(m_printf, snprintfUnknownSpecifier)
{
    char out[16];
    uint32_t len = m_snprintf_0(out, sizeof(out), "a%0q");

    EXPECT_STREQ(out, "a");
    EXPECT_EQ(len, 1U);
}

TEST(m_printf, snprintfMissingParameter)
{
    char out[32];
    uint32_t len = m_snprintf_1(out, sizeof(out), "hello %d %d", 42);

    EXPECT_STREQ(out, "hello 42 ");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, printfNullFormat)
{
    EXPECT_EQ(m_printf_0(NULL), 0U);
}

TEST(m_printf, snprintfNullDest)
{
    uint32_t len = mSnprintf_general(NULL, 0U, "OK", 0U, 0ULL, 0ULL, 0ULL, 0ULL,
                                     0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL);

    EXPECT_EQ(len, 0U);
}

TEST(m_printf, CustomSnprintfOverflow)
{
    char out[8];
    uint32_t len = m_snprintf_0(out, sizeof(out), "ABCDEFGHJKLMN");

    EXPECT_STREQ(out, "ABCDEFG");
    EXPECT_EQ(len, 7U);
}

TEST(m_printf, snprintfBufferOverflowTruncates)
{
    char out[5];
    uint32_t len = m_snprintf_0(out, sizeof(out), "123456789");

    EXPECT_STREQ(out, "1234");
    EXPECT_EQ(len, 4U);
}

TEST(m_printf, CustomPrintfCallback)
{
    s_capture_len       = 0;
    s_capture_buffer[0] = '\0';

    m_printf_setOutput(m_printf_capture);
    m_printf_3("X %u %s %c%%", 123U, "abc", 'Q');
    m_printf_setOutput(NULL);

    EXPECT_STREQ(s_capture_buffer, "X 123 abc Q%");
    EXPECT_EQ(s_capture_len, 12U);
}

TEST(m_printf, PrintfZeroParameters)
{
    s_capture_len       = 0;
    s_capture_buffer[0] = '\0';

    m_printf_setOutput(m_printf_capture);
    m_printf_0("OK");
    m_printf_setOutput(NULL);

    EXPECT_STREQ(s_capture_buffer, "OK");
    EXPECT_EQ(s_capture_len, 2U);
}

TEST(m_printf, SnprintfPointerAndLong)
{
    char out[64];
    uint32_t len = m_snprintf_2(out, sizeof(out), "%p %ld", (void*)0,
                                (long)-1234);

    EXPECT_STREQ(out, "0x0 -1234");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}

TEST(m_printf, SnprintfAliasWrapper)
{
    char out[32];

    uint32_t len = m_snprintf_1(out, sizeof(out), "%u", 65535U);

    EXPECT_STREQ(out, "65535");
    EXPECT_EQ(len, (uint32_t)strlen(out));
}
