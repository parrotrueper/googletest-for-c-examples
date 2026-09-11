#include <gtest/gtest.h>

#include "mStrToNum.h"

using namespace testing;

/* matoi_s32 */
class matoiTestFixture
    : public TestWithParam<std::tuple<const char_t*, uint8_t, int32_t, bool>>
{
};

TEST_P(matoiTestFixture, matoiTest)
{
    const char_t* str   = std::get<0>(GetParam());
    uint8_t length      = std::get<1>(GetParam());
    int32_t expected    = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());

    int32_t value;
    bool valid = matoi_s32(str, length, &value);
    EXPECT_EQ(valid, expected_valid);
    if ( valid ) {
        EXPECT_EQ(value, expected);
    }
}
INSTANTIATE_TEST_SUITE_P(
    matoiTests, matoiTestFixture,
    Values(std::make_tuple((const char_t*)("123"), 3, 123, true),
           std::make_tuple((const char_t*)("-456"), 4, -456, true),
           std::make_tuple((const char_t*)("0"), 1, 0, true),
           std::make_tuple((const char_t*)("abc"), 3, 0, false),
           std::make_tuple((const char_t*)("12a"), 3, 0, false),
           std::make_tuple((const char_t*)(""), 0, 0, false),
           std::make_tuple((const char_t*)("2147483647"), 10, 2147483647,
                           true), /* max int  */
           std::make_tuple((const char_t*)("-2147483648"), 11, -2147483648,
                           true), /* min int */
           std::make_tuple((const char_t*)("2147483648"), 10, 0,
                           false), /* overflow */
           std::make_tuple((const char_t*)("-2147483649"), 11, 0,
                           false) /* underflow */));

TEST(matoiTest, NullStringFails)
{
    int32_t value = 0;
    EXPECT_FALSE(matoi_s32(NULL, 1, &value));
}

TEST(matoiTest, NullOutputPointerFails)
{
    const char_t* str = (const char_t*)("123");
    EXPECT_FALSE(matoi_s32(str, 3, NULL));
}

TEST(matoiTest, ZeroLengthFails)
{
    const char_t* str = (const char_t*)("123");
    int32_t value     = 0;
    EXPECT_FALSE(matoi_s32(str, 0, &value));
}

TEST(matoiTest, LeadingPlusSignFails)
{
    const char_t* str = (const char_t*)("+123");
    int32_t value     = 0;
    EXPECT_FALSE(matoi_s32(str, 4, &value));
}

TEST(matoiTest, ShortLengthParsesPrefix)
{
    const char_t* str = (const char_t*)("12345");
    int32_t value     = 0;
    EXPECT_TRUE(matoi_s32(str, 3, &value));
    EXPECT_EQ(value, 123);
}

/* matoi_u16 */
class matoiU16TestFixture
    : public TestWithParam<std::tuple<const char_t*, uint8_t, uint16_t, bool>>
{
};
TEST_P(matoiU16TestFixture, matoiU16Test)
{
    const char_t* str   = std::get<0>(GetParam());
    uint8_t length      = std::get<1>(GetParam());
    uint16_t expected   = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());

    uint16_t value;
    bool valid = matoi_u16(str, length, &value);
    EXPECT_EQ(valid, expected_valid);
    if ( valid ) {
        EXPECT_EQ(value, expected);
    }
}
INSTANTIATE_TEST_SUITE_P(
    matoiU16Tests, matoiU16TestFixture,
    Values(std::make_tuple((const char_t*)("123"), 3, 123, true),
           std::make_tuple((const char_t*)("0"), 1, 0, true),
           std::make_tuple((const char_t*)("65535"), 5, 65535,
                           true), /* max uint16_t */
           std::make_tuple((const char_t*)("65536"), 5, 0,
                           false), /* overflow */
           std::make_tuple((const char_t*)("abc"), 3, 0, false),
           std::make_tuple((const char_t*)("12a"), 3, 0, false),
           std::make_tuple((const char_t*)(""), 0, 0, false),
           std::make_tuple((const char_t*)("-1"), 2, 0,
                           false), /* negative number */
           std::make_tuple((const char_t*)(" 123"), 4, 0,
                           false), /* leading space */
           std::make_tuple((const char_t*)("123 "), 4, 0,
                           false), /* trailing space */
           std::make_tuple((const char_t*)("12 3"), 4, 0,
                           false) /* space in the middle */));

/* matoi_u32 */
class matoiU32TestFixture
    : public TestWithParam<std::tuple<const char_t*, uint8_t, uint32_t, bool>>
{
};
TEST_P(matoiU32TestFixture, matoiU32Test)
{
    const char_t* str   = std::get<0>(GetParam());
    uint8_t length      = std::get<1>(GetParam());
    uint32_t expected   = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());

    uint32_t value;
    bool valid = matoi_u32(str, length, &value);
    EXPECT_EQ(valid, expected_valid);
    if ( valid ) {
        EXPECT_EQ(value, expected);
    }
}
INSTANTIATE_TEST_SUITE_P(
    matoiU32Tests, matoiU32TestFixture,
    Values(std::make_tuple((const char_t*)("123"), 3, 123U, true),
           std::make_tuple((const char_t*)("0"), 1, 0U, true),
           std::make_tuple((const char_t*)("4294967295"), 10, 4294967295U,
                           true), /* max uint32_t */
           std::make_tuple((const char_t*)("4294967296"), 10, 0U,
                           false), /* overflow */
           std::make_tuple((const char_t*)("abc"), 3, 0U, false),
           std::make_tuple((const char_t*)("12a"), 3, 0U, false),
           std::make_tuple((const char_t*)(""), 0, 0U, false),
           std::make_tuple((const char_t*)("-1"), 2, 0U,
                           false), /* negative number */
           std::make_tuple((const char_t*)(" 123"), 4, 0U,
                           false), /* leading space */
           std::make_tuple((const char_t*)("123 "), 4, 0U,
                           false), /* trailing space */
           std::make_tuple((const char_t*)("12 3"), 4, 0U,
                           false) /* space in the middle */));

/* matoi_u8 */
class matoiU8TestFixture
    : public TestWithParam<std::tuple<const char_t*, uint8_t, uint8_t, bool>>
{
};
TEST_P(matoiU8TestFixture, matoiU8Test)
{
    const char_t* str   = std::get<0>(GetParam());
    uint8_t length      = std::get<1>(GetParam());
    uint8_t expected    = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());
    uint8_t value;
    bool valid = matoi_u8(str, length, &value);
    EXPECT_EQ(valid, expected_valid);
    if ( valid ) {
        EXPECT_EQ(value, expected);
    }
}

INSTANTIATE_TEST_SUITE_P(
    matoiU8Tests, matoiU8TestFixture,
    Values(std::make_tuple((const char_t*)("123"), 3, 123, true),
           std::make_tuple((const char_t*)("0"), 1, 0, true),
           std::make_tuple((const char_t*)("255"), 3, 255,
                           true), /* max uint8_t */
           std::make_tuple((const char_t*)("256"), 3, 0, false), /* overflow */
           std::make_tuple((const char_t*)("abc"), 3, 0, false),
           std::make_tuple((const char_t*)("12a"), 3, 0, false),
           std::make_tuple((const char_t*)(""), 0, 0, false),
           std::make_tuple((const char_t*)("-1"), 2, 0,
                           false), /* negative number */
           std::make_tuple((const char_t*)(" 123"), 4, 0,
                           false), /* leading space */
           std::make_tuple((const char_t*)("123 "), 4, 0,
                           false), /* trailing space */
           std::make_tuple((const char_t*)("12 3"), 4, 0,
                           false) /* space in the middle */));
