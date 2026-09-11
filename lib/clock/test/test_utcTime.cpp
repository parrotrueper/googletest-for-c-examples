#include <array>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>

#include "utcNorm.h"
#include "utcTime.h"
#include "utilDevDebug.h"

using namespace testing;

/* all valid timezones */
static st_utc_zone_t valid_time_zones[] = {
    {'Z', 0, 0},  {'-', 12, 0},  {'-', 11, 0}, {'-', 10, 0},  {'-', 9, 30},
    {'-', 9, 0},  {'-', 8, 0},   {'-', 7, 0},  {'-', 6, 0},   {'-', 5, 0},
    {'-', 4, 0},  {'-', 3, 30},  {'-', 3, 0},  {'-', 2, 0},   {'-', 1, 0},
    {'+', 0, 0},  {'+', 1, 0},   {'+', 2, 0},  {'+', 3, 0},   {'+', 3, 30},
    {'+', 4, 0},  {'+', 4, 30},  {'+', 5, 0},  {'+', 5, 30},  {'+', 5, 45},
    {'+', 6, 0},  {'+', 6, 30},  {'+', 7, 0},  {'+', 8, 0},   {'+', 8, 45},
    {'+', 9, 0},  {'+', 9, 30},  {'+', 10, 0}, {'+', 10, 30}, {'+', 11, 0},
    {'+', 12, 0}, {'+', 12, 45}, {'+', 13, 0}, {'+', 14, 0}};

/* -- from String to utc */
class utcTimeFromBufferTestFixture
    : public testing::TestWithParam<
          std::tuple<std::string, st_utc_time_t, bool>>
{
};
TEST_P(utcTimeFromBufferTestFixture, utcTimeFromBufferTest)
{
    std::string buffer_str = std::get<0>(GetParam());
    st_utc_time_t expected = std::get<1>(GetParam());
    bool expected_valid    = std::get<2>(GetParam());
    bool valid             = utcTime_fromString(
        reinterpret_cast<const uint8_t*>(buffer_str.c_str()),
        static_cast<uint16_t>(buffer_str.length()), &expected);
    EXPECT_EQ(valid, expected_valid);
    if ( valid ) {
        st_utc_time_t actual;
        valid = utcTime_fromString(
            reinterpret_cast<const uint8_t*>(buffer_str.c_str()),
            static_cast<uint16_t>(buffer_str.length()), &actual);
        EXPECT_EQ(valid, expected_valid);
        if ( valid ) {
            EXPECT_EQ(actual.year, expected.year);
            EXPECT_EQ(actual.month, expected.month);
            EXPECT_EQ(actual.day, expected.day);
            EXPECT_EQ(actual.hour, expected.hour);
            EXPECT_EQ(actual.minute, expected.minute);
            EXPECT_EQ(actual.second, expected.second);
            EXPECT_EQ(actual.microseconds, expected.microseconds);
            EXPECT_EQ(actual.zone.sign, expected.zone.sign);
            EXPECT_EQ(actual.zone.hrs, expected.zone.hrs);
            EXPECT_EQ(actual.zone.mins, expected.zone.mins);
        }
    }
}
INSTANTIATE_TEST_SUITE_P(
    utcTimeFromBufferTests, utcTimeFromBufferTestFixture,
    Values(
        /* Zulu time: seconds, milliseconds, microseconds */
        std::make_tuple("2024-02-29T12:34:56Z",
                        st_utc_time_t{2024, 2, 29, 12, 34, 56, 0, {'Z', 0, 0}},
                        true),
        std::make_tuple(
            "2024-02-29T12:34:56.123Z",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123000, {'Z', 0, 0}}, true),
        std::make_tuple(
            "2024-02-29T12:34:56.123456Z",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123456, {'Z', 0, 0}}, true),

        /* +00:00 time: seconds, milliseconds, microseconds */
        std::make_tuple("2024-02-29T12:34:56+00:00",
                        st_utc_time_t{2024, 2, 29, 12, 34, 56, 0, {'+', 0, 0}},
                        true),
        std::make_tuple(
            "2024-02-29T12:34:56.123+00:00",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123000, {'+', 0, 0}}, true),
        std::make_tuple(
            "2024-02-29T12:34:56.123456+00:00",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123456, {'+', 0, 0}}, true),

        /* Z time: seconds, milliseconds, microseconds */
        std::make_tuple("2024-02-29T12:34:56Z",
                        st_utc_time_t{2024, 2, 29, 12, 34, 56, 0, {'Z', 0, 0}},
                        true),
        std::make_tuple(
            "2024-02-29T12:34:56.123Z",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123000, {'Z', 0, 0}}, true),
        std::make_tuple(
            "2024-02-29T12:34:56.123456Z",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123456, {'Z', 0, 0}}, true),

        /* -05:00 time: seconds, milliseconds, microseconds */
        std::make_tuple("2024-02-29T12:34:56-05:00",
                        st_utc_time_t{2024, 2, 29, 12, 34, 56, 0, {'-', 5, 0}},
                        true),
        std::make_tuple(
            "2024-02-29T12:34:56.123-05:00",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123000, {'-', 5, 0}}, true),
        std::make_tuple(
            "2024-02-29T12:34:56.123456-05:00",
            st_utc_time_t{2024, 2, 29, 12, 34, 56, 123456, {'-', 5, 0}}, true),
        std::make_tuple("2024-02-29 12:34:56Z",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024/02/29T12:34:56Z",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12.34:56Z",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12:34:56+0000",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12:34:56+00:0",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12:34:56",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12:34:56.Z",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, false),
        std::make_tuple("2024-02-29T12:34:56.123Z",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}}, true),
        std::make_tuple("2024-02-29T12:34:56Zabc",
                        st_utc_time_t{0, 0, 0, 0, 0, 0, 0, {'Z', 0, 0}},
                        false)));

TEST(UtcTimeTest, utcTimeFromBufferParsesAllValidTimezones)
{
    st_utc_time_t expected = {2024, 2, 29, 12, 34, 56, 0, {'Z', 0, 0}};
    char buffer[32];

    for ( const st_utc_zone_t& zone : valid_time_zones ) {
        if ( zone.sign == 'Z' ) {
            std::strcpy(buffer, "2024-02-29T12:34:56Z");
        } else {
            std::sprintf(buffer, "2024-02-29T12:34:56%c%02u:%02u", zone.sign,
                         zone.hrs, zone.mins);
        }

        st_utc_time_t actual = expected;
        actual.zone          = zone;
        bool valid           = utcTime_fromString(
            reinterpret_cast<const uint8_t*>(buffer),
            static_cast<uint16_t>(std::strlen(buffer)), &actual);

        EXPECT_TRUE(valid) << buffer;
        if ( valid ) {
            EXPECT_EQ(actual.year, expected.year) << buffer;
            EXPECT_EQ(actual.month, expected.month) << buffer;
            EXPECT_EQ(actual.day, expected.day) << buffer;
            EXPECT_EQ(actual.hour, expected.hour) << buffer;
            EXPECT_EQ(actual.minute, expected.minute) << buffer;
            EXPECT_EQ(actual.second, expected.second) << buffer;
            EXPECT_EQ(actual.microseconds, expected.microseconds) << buffer;
            EXPECT_EQ(actual.zone.sign, zone.sign) << buffer;
            EXPECT_EQ(actual.zone.hrs, zone.hrs) << buffer;
            EXPECT_EQ(actual.zone.mins, zone.mins) << buffer;
        }
    }
}

/* -- utc is greater -- */
TEST(UtcTimeTest, UtcTimeIsGreaterComparesDateTimeAndOffset)
{
    st_utc_time_t earlier = {2024, 2, 29, 12, 0, 0, 0, {'Z', 0, 0}};
    st_utc_time_t later   = {2024, 2, 29, 12, 0, 1, 0, {'Z', 0, 0}};
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));
    EXPECT_FALSE(utcTime_isGreater(&earlier, &later));

    later.second = 0;
    later.minute = 1;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));
    EXPECT_FALSE(utcTime_isGreater(&earlier, &later));

    later.minute = 0;
    later.hour   = 13;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));

    later.hour = 12;
    later.day  = 30;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));

    later.day   = 29;
    later.month = 3;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));

    later.month = 2;
    later.year  = 2025;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));

    later.year      = earlier.year;
    later.month     = earlier.month;
    later.day       = earlier.day;
    later.hour      = earlier.hour;
    later.minute    = earlier.minute;
    later.second    = earlier.second;
    later.zone.sign = '-';
    later.zone.hrs  = 1;
    later.zone.mins = 0;
    EXPECT_TRUE(utcTime_isGreater(&later, &earlier));
    EXPECT_FALSE(utcTime_isGreater(&earlier, &later));

    later.zone.sign = '+';
    later.zone.hrs  = 1;
    later.zone.mins = 0;
    EXPECT_FALSE(utcTime_isGreater(&later, &earlier));
    EXPECT_TRUE(utcTime_isGreater(&earlier, &later));

    st_utc_time_t same_instant1 = {2024, 2, 29, 12, 0, 0, 0, {'+', 2, 0}};
    st_utc_time_t same_instant2 = {2024, 2, 29, 10, 0, 0, 0, {'Z', 0, 0}};
    EXPECT_FALSE(utcTime_isGreater(&same_instant1, &same_instant2));
    EXPECT_FALSE(utcTime_isGreater(&same_instant2, &same_instant1));

    st_utc_time_t cross_midnight1 = {2024, 3, 1, 0, 30, 0, 0, {'+', 2, 0}};
    st_utc_time_t cross_midnight2 = {2024, 2, 29, 23, 0, 0, 0, {'Z', 0, 0}};
    EXPECT_FALSE(utcTime_isGreater(&cross_midnight1, &cross_midnight2));
    EXPECT_TRUE(utcTime_isGreater(&cross_midnight2, &cross_midnight1));

    st_utc_time_t micro1 = {2024, 2, 29, 12, 0, 0, 1, {'Z', 0, 0}};
    st_utc_time_t micro2 = {2024, 2, 29, 12, 0, 0, 0, {'Z', 0, 0}};
    EXPECT_TRUE(utcTime_isGreater(&micro1, &micro2));
    EXPECT_FALSE(utcTime_isGreater(&micro2, &micro1));

    EXPECT_FALSE(utcTime_isGreater(NULL, &earlier));
    EXPECT_FALSE(utcTime_isGreater(&later, NULL));
}
/* -- utc copy -- */
TEST(utcTimeCopyTest, copy)
{
    st_utc_time_t src = {2026, 1, 1, 0, 0, 0, 0, {'+', 3, 30}};
    st_utc_time_t dst = {99, 99, 99, 99, 99, 99, 99, {'-', 99, 99}};

    utcTime_copy(&src, &dst);

    EXPECT_EQ(src.year, dst.year);
    EXPECT_EQ(src.month, dst.month);
    EXPECT_EQ(src.day, dst.day);
    EXPECT_EQ(src.hour, dst.hour);
    EXPECT_EQ(src.minute, dst.minute);
    EXPECT_EQ(src.second, dst.second);
    EXPECT_EQ(src.microseconds, dst.microseconds);
    EXPECT_EQ(src.zone.sign, dst.zone.sign);
    EXPECT_EQ(src.zone.hrs, dst.zone.hrs);
    EXPECT_EQ(src.zone.mins, dst.zone.mins);
}
/* -- utc to buffer -- */
class utcTimeToBufferTestFixture
    : public testing::TestWithParam<
          std::tuple<st_utc_time_t, e_utc_format_t, std::string, bool>>
{
};
TEST_P(utcTimeToBufferTestFixture, utcTimeToBufferTest)
{
    st_utc_time_t utc_time = std::get<0>(GetParam());
    e_utc_format_t format  = std::get<1>(GetParam());
    std::string expected   = std::get<2>(GetParam());
    bool expected_valid    = std::get<3>(GetParam());

    uint8_t buffer[36] = {0};
    bool valid = utcTime_toBuffer(&utc_time, format, buffer, sizeof(buffer));

    EXPECT_EQ(valid, expected_valid);
    DD_TRACE("utcTime_toBuffer: %s\n", buffer);
    if ( valid ) {
        EXPECT_STREQ(reinterpret_cast<const char*>(buffer), expected.c_str());
    }
}
INSTANTIATE_TEST_SUITE_P(
    UtcTimeToBufferTests, utcTimeToBufferTestFixture,
    Values(
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'+', 3, 30}},
                        eUtcSeconds, "2026-01-01T00:00:00+03:30", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'Z', 0, 0}},
                        eUtcSeconds, "2026-01-01T00:00:00+00:00", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'+', 0, 0}},
                        eUtcSeconds, "2026-01-01T00:00:00+00:00", true),

        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'-', 9, 30}},
                        eUtcSecondsZ, "2026-01-01T00:00:00-09:30", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'Z', 0, 0}},
                        eUtcSecondsZ, "2026-01-01T00:00:00Z", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 0, {'+', 0, 0}},
                        eUtcSecondsZ, "2026-01-01T00:00:00Z", true),

        std::make_tuple(
            st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'+', 5, 30}},
            eUtcMilliseconds, "2026-01-01T00:00:00.123+05:30", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'Z', 0, 0}},
                        eUtcMilliseconds, "2026-01-01T00:00:00.123+00:00",
                        true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'+', 0, 0}},
                        eUtcMilliseconds, "2026-01-01T00:00:00.123+00:00",
                        true),

        std::make_tuple(
            st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'-', 3, 30}},
            eUtcMillisecondsZ, "2026-01-01T00:00:00.123-03:30", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'Z', 0, 0}},
                        eUtcMillisecondsZ, "2026-01-01T00:00:00.123Z", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'+', 0, 0}},
                        eUtcMillisecondsZ, "2026-01-01T00:00:00.123Z", true),


        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'-', 5, 0}},
                        eUtcMicroseconds, "2026-01-01T00:00:00.123456-05:00",
                        true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'Z', 0, 0}},
                        eUtcMicroseconds, "2026-01-01T00:00:00.123456+00:00",
                        true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'+', 0, 0}},
                        eUtcMicroseconds, "2026-01-01T00:00:00.123456+00:00",
                        true),

        std::make_tuple(
            st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'-', 3, 30}},
            eUtcMicrosecondsZ, "2026-01-01T00:00:00.123456-03:30", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'Z', 0, 0}},
                        eUtcMicrosecondsZ, "2026-01-01T00:00:00.123456Z", true),
        std::make_tuple(st_utc_time_t{2026, 1, 1, 0, 0, 0, 123456, {'+', 0, 0}},
                        eUtcMicrosecondsZ, "2026-01-01T00:00:00.123456Z",
                        true)));

static std::string utcTimeToBufferExpected(const st_utc_time_t& utc_time,
                                           e_utc_format_t format)
{
    bool is_zone_z = (utc_time.zone.sign == 'Z')
                     || ((utc_time.zone.sign == '+') && (utc_time.zone.hrs == 0)
                         && (utc_time.zone.mins == 0));
    char zone_buffer[16] = {0};

    if ( is_zone_z
         && (format == eUtcSecondsZ || format == eUtcMillisecondsZ
             || format == eUtcMicrosecondsZ) ) {
        std::strcpy(zone_buffer, "Z");
    } else if ( utc_time.zone.sign == 'Z' ) {
        std::strcpy(zone_buffer, "+00:00");
    } else {
        std::snprintf(zone_buffer, sizeof(zone_buffer), "%c%02u:%02u",
                      utc_time.zone.sign, utc_time.zone.hrs,
                      utc_time.zone.mins);
    }

    char output[64] = {0};
    if ( format == eUtcSeconds || format == eUtcSecondsZ ) {
        std::sprintf(output, "%04u-%02u-%02uT%02u:%02u:%02u%s", utc_time.year,
                     utc_time.month, utc_time.day, utc_time.hour,
                     utc_time.minute, utc_time.second, zone_buffer);
    } else if ( format == eUtcMilliseconds || format == eUtcMillisecondsZ ) {
        std::sprintf(output, "%04u-%02u-%02uT%02u:%02u:%02u.%03u%s",
                     utc_time.year, utc_time.month, utc_time.day, utc_time.hour,
                     utc_time.minute, utc_time.second,
                     utc_time.microseconds / 1000, zone_buffer);
    } else {
        std::sprintf(output, "%04u-%02u-%02uT%02u:%02u:%02u.%06u%s",
                     utc_time.year, utc_time.month, utc_time.day, utc_time.hour,
                     utc_time.minute, utc_time.second, utc_time.microseconds,
                     zone_buffer);
    }
    return std::string(output);
}

TEST(UtcTimeTest, utcTimeToBufferCoversAllValidTimezones)
{
    st_utc_time_t utc_time = {2026, 1, 1, 12, 34, 56, 123456, {'Z', 0, 0}};
    const e_utc_format_t formats[] = {
        eUtcSeconds,       eUtcSecondsZ,     eUtcMilliseconds,
        eUtcMillisecondsZ, eUtcMicroseconds, eUtcMicrosecondsZ,
    };

    for ( const st_utc_zone_t& zone : valid_time_zones ) {
        utc_time.zone = zone;
        for ( e_utc_format_t format : formats ) {
            uint8_t buffer[36] = {0};
            bool valid         = utcTime_toBuffer(&utc_time, format, buffer,
                                                  sizeof(buffer));
            EXPECT_TRUE(valid);
            if ( valid ) {
                std::string expected = utcTimeToBufferExpected(utc_time,
                                                               format);
                EXPECT_STREQ(expected.c_str(),
                             reinterpret_cast<const char*>(buffer));
            }
        }
    }
}

TEST(UtcTimeTest, utcTimeToBufferZeroOffsetFormats)
{
    st_utc_zone_t zero_offsets[] = {{'Z', 0, 0}, {'+', 0, 0}};
    st_utc_time_t utc_time = {2026, 1, 1, 12, 34, 56, 123456, {'Z', 0, 0}};
    const e_utc_format_t formats[] = {
        eUtcSeconds,       eUtcSecondsZ,     eUtcMilliseconds,
        eUtcMillisecondsZ, eUtcMicroseconds, eUtcMicrosecondsZ,
    };

    for ( const st_utc_zone_t& zone : zero_offsets ) {
        utc_time.zone = zone;
        for ( e_utc_format_t format : formats ) {
            uint8_t buffer[36] = {0};
            bool valid         = utcTime_toBuffer(&utc_time, format, buffer,
                                                  sizeof(buffer));
            EXPECT_TRUE(valid);
            if ( valid ) {
                std::string expected = utcTimeToBufferExpected(utc_time,
                                                               format);
                EXPECT_STREQ(expected.c_str(),
                             reinterpret_cast<const char*>(buffer));
            }
        }
    }
}

/* -- valid date -- COPIED from test_dateTime.cpp */
class utcValidDateTestFixture
    : public testing::TestWithParam<
          std::tuple<uint16_t, uint8_t, uint8_t, bool>>
{
};

TEST_P(utcValidDateTestFixture, validDateTest)
{
    uint16_t year       = std::get<0>(GetParam());
    uint8_t month       = std::get<1>(GetParam());
    uint8_t day         = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());

    bool valid = utcTime_isValidDate(year, month, day);
    EXPECT_EQ(valid, expected_valid);
}

INSTANTIATE_TEST_SUITE_P(
    ValidDateTests, utcValidDateTestFixture,
    Values(std::make_tuple(2020, 1, 31, true),   /* Jan */
           std::make_tuple(2020, 2, 29, true),   /* Feb leap year */
           std::make_tuple(2021, 2, 28, true),   /* Feb not leap year */
           std::make_tuple(2020, 2, 30, false),  /* Feb not leap year */
           std::make_tuple(2020, 4, 30, true),   /* Apr */
           std::make_tuple(2020, 4, 31, false),  /* Apr */
           std::make_tuple(2020, 12, 31, true),  /* Dec */
           std::make_tuple(2020, 12, 32, false), /* Dec */
           std::make_tuple(1600, 2, 29, true),   /* leap year */
           std::make_tuple(40, 2, 29, true),     /* leap year */
           /* out of bounds values */
           std::make_tuple(2020, 0, 15, false),  /* month out of bounds */
           std::make_tuple(2020, 13, 15, false), /* month out of bounds */
           std::make_tuple(2020, 1, 0, false),   /* day out of bounds */
           std::make_tuple(2020, 1, 32, false),  /* day out of bounds */
           /* minimum values */
           std::make_tuple(0, 1, 1, true),  /* minimum valid date */
           std::make_tuple(0, 0, 0, false), /* minimum out of bounds */
           /* maximum values */
           std::make_tuple(65535, 12, 31, true), /* maximum valid date */
           std::make_tuple(65535, 12, 32, false) /* maximum out of bounds */
           ));

/* -- valid date time, valid utc --- */
class utcValidDateTimeTestFixture
    : public testing::TestWithParam<
          std::tuple<uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t,
                     uint32_t, char_t, uint8_t, uint8_t, bool>>
{
};

TEST_P(utcValidDateTimeTestFixture, validDateTimeTest)
{
    uint16_t year         = std::get<0>(GetParam());
    uint8_t month         = std::get<1>(GetParam());
    uint8_t day           = std::get<2>(GetParam());
    uint8_t hour          = std::get<3>(GetParam());
    uint8_t minute        = std::get<4>(GetParam());
    uint8_t second        = std::get<5>(GetParam());
    uint32_t microseconds = std::get<6>(GetParam());
    st_utc_zone_t zone;
    zone.sign              = std::get<7>(GetParam());
    zone.hrs               = std::get<8>(GetParam());
    zone.mins              = std::get<9>(GetParam());
    st_utc_time_t utc_time = {year,   month,  day,          hour,
                              minute, second, microseconds, zone};
    bool expected_valid    = std::get<10>(GetParam());


    bool valid = utcTime_isValidDateTime(year, month, day, hour, minute, second,
                                         microseconds, zone);
    EXPECT_EQ(valid, expected_valid);
    valid = utcTime_isValidUtc(&utc_time);
    EXPECT_EQ(valid, expected_valid);
}

INSTANTIATE_TEST_SUITE_P(
    ValidDateTests, utcValidDateTimeTestFixture,
    Values(std::make_tuple(2026, 1, 1, 0, 0, 0, 0, '+', 0, 0, true),
           /* 2026 not leap year */
           std::make_tuple(2026, 2, 29, 0, 0, 0, 0, '+', 0, 0, false),
           std::make_tuple(2024, 2, 29, 0, 0, 0, 0, '+', 0, 0, true),
           std::make_tuple(2026, 4, 30, 0, 0, 0, 0, '+', 0, 0, true),
           /* April has 30 days */
           std::make_tuple(2026, 4, 31, 0, 0, 0, 0, '+', 0, 0, false),
           std::make_tuple(2026, 12, 31, 0, 0, 0, 0, '+', 0, 0, true),
           /* invalid month */
           std::make_tuple(2026, 0, 10, 0, 0, 0, 0, '+', 0, 0, false),
           /* invalid month */
           std::make_tuple(2026, 13, 10, 0, 0, 0, 0, '+', 0, 0, false),
           /* invalid day */
           std::make_tuple(2026, 1, 0, 0, 0, 0, 0, '+', 0, 0, false),
           /* invalid day */
           std::make_tuple(2026, 1, 32, 0, 0, 0, 0, '+', 0, 0, false),
           /* invalid hour */
           std::make_tuple(2026, 1, 31, 24, 0, 0, 0, '+', 0, 0, false),
           /* invalid minutes */
           std::make_tuple(2026, 1, 31, 23, 60, 0, 0, '+', 0, 0, false),
           /* invalid seconds */
           std::make_tuple(2026, 1, 31, 23, 59, 60, 0, '+', 0, 0, false),
           /* invalid microseconds */
           std::make_tuple(2026, 1, 31, 23, 59, 59, 1000000, '+', 0, 0, false),
           /* invalid offset */
           std::make_tuple(2026, 1, 31, 23, 59, 60, 0, 'A', 0, 0, false),
           std::make_tuple(2026, 1, 31, 23, 59, 60, 0, '+', 20, 0, false),
           std::make_tuple(2026, 1, 31, 23, 59, 60, 0, '+', 0, 60, false),
           std::make_tuple(2026, 1, 1, 0, 0, 0, 0, '-', 12, 30, false),
           /* valid minimum value */
           std::make_tuple(2026, 1, 1, 0, 0, 0, 0, '-', 12, 0, true)));


/* --- time zone --- */
class validTimeZoneTestFixture
    : public testing::TestWithParam<std::tuple<char_t, uint8_t, uint8_t, bool>>
{
};

TEST_P(validTimeZoneTestFixture, validTimeZoneTest)
{
    char_t sign         = std::get<0>(GetParam());
    uint8_t hours       = std::get<1>(GetParam());
    uint8_t minutes     = std::get<2>(GetParam());
    bool expected_valid = std::get<3>(GetParam());

    st_utc_zone_t zone;
    zone.sign  = sign;
    zone.hrs   = hours;
    zone.mins  = minutes;
    bool valid = utcTime_isTimeZoneValid(&zone);
    EXPECT_EQ(valid, expected_valid);
}
/* clang-format off */
INSTANTIATE_TEST_SUITE_P(
    validTimeZoneTests, validTimeZoneTestFixture,
    Values( /* all valid cases */
        std::make_tuple('-', 12, 0, true), std::make_tuple('-', 11, 0, true),
        std::make_tuple('-', 10, 0, true), std::make_tuple('-', 9, 30, true),
        std::make_tuple('-', 9, 0, true), std::make_tuple('-', 8, 0, true),
        std::make_tuple('-', 7, 0, true), std::make_tuple('-', 6, 0, true),
        std::make_tuple('-', 5, 0, true), std::make_tuple('-', 4, 0, true),
        std::make_tuple('-', 3, 30, true), std::make_tuple('-', 3, 0, true),
        std::make_tuple('-', 2, 0, true), std::make_tuple('-', 1, 0, true),
        std::make_tuple('Z', 0, 0, true), std::make_tuple('+', 0, 0, true),
        std::make_tuple('+', 1, 0, true), std::make_tuple('+', 2, 0, true),
        std::make_tuple('+', 3, 0, true), std::make_tuple('+', 3, 30, true),
        std::make_tuple('+', 4, 0, true), std::make_tuple('+', 4, 30, true),
        std::make_tuple('+', 5, 0, true), std::make_tuple('+', 5, 30, true),
        std::make_tuple('+', 5, 45, true), std::make_tuple('+', 6, 0, true),
        std::make_tuple('+', 6, 30, true), std::make_tuple('+', 7, 0, true),
        std::make_tuple('+', 8, 0, true), std::make_tuple('+', 8, 45, true),
        std::make_tuple('+', 9, 0, true), std::make_tuple('+', 9, 30, true),
        std::make_tuple('+', 10, 0, true), std::make_tuple('+', 10, 30, true),
        std::make_tuple('+', 11, 0, true), std::make_tuple('+', 12, 0, true),
        std::make_tuple('+', 12, 45, true), std::make_tuple('+', 13, 0, true),
        std::make_tuple('+', 14, 0, true),
        /* invalid cases */
        std::make_tuple('-',12,30, false), std::make_tuple('+', 12, 30, false),
        std::make_tuple('+', 14, 30, false),std::make_tuple('+', 15,00, false)
    ));
/* clang-format on */
/* -- dud date -- */
TEST(UtcTimeTest, setsDudDateTo1999_01_01)
{
    st_utc_time_t utc_time;
    utcTime_setDudDate(&utc_time);

    EXPECT_EQ(utc_time.year, 1999);
    EXPECT_EQ(utc_time.month, 1);
    EXPECT_EQ(utc_time.day, 1);
    EXPECT_EQ(utc_time.hour, 0);
    EXPECT_EQ(utc_time.minute, 0);
    EXPECT_EQ(utc_time.second, 0);
    EXPECT_EQ(utc_time.zone.sign, '+');
    EXPECT_EQ(utc_time.zone.hrs, 0);
    EXPECT_EQ(utc_time.zone.mins, 0);
    EXPECT_EQ(utc_time.microseconds, 0);
}
/* -- normalise to UTC Z -- */
TEST(UtcTimeTest, normalisesPositiveOffsetAcrossMonthBoundary)
{
    st_utc_time_t utc_time = {2024, 1, 31, 0, 0, 0, 0, {'+', 2, 0}};

    utcNorm_toUtcZ(&utc_time);

    EXPECT_EQ(utc_time.year, 2024);
    EXPECT_EQ(utc_time.month, 1);
    EXPECT_EQ(utc_time.day, 30);
    EXPECT_EQ(utc_time.hour, 22);
    EXPECT_EQ(utc_time.minute, 0);
    EXPECT_EQ(utc_time.zone.sign, 'Z');
    EXPECT_EQ(utc_time.zone.hrs, 0);
    EXPECT_EQ(utc_time.zone.mins, 0);
}

TEST(UtcTimeTest, normalisesPositiveOffsetAcrossYearBoundary)
{
    st_utc_time_t utc_time = {2020, 1, 1, 0, 30, 0, 0, {'+', 14, 0}};

    utcNorm_toUtcZ(&utc_time);

    EXPECT_EQ(utc_time.year, 2019);
    EXPECT_EQ(utc_time.month, 12);
    EXPECT_EQ(utc_time.day, 31);
    EXPECT_EQ(utc_time.hour, 10);
    EXPECT_EQ(utc_time.minute, 30);
    EXPECT_EQ(utc_time.zone.sign, 'Z');
    EXPECT_EQ(utc_time.zone.hrs, 0);
    EXPECT_EQ(utc_time.zone.mins, 0);
}

TEST(UtcTimeTest, normalisesNegativeOffsetWithinSameDay)
{
    st_utc_time_t utc_time = {2026, 1, 15, 5, 0, 0, 0, {'-', 5, 0}};

    utcNorm_toUtcZ(&utc_time);

    EXPECT_EQ(utc_time.year, 2026);
    EXPECT_EQ(utc_time.month, 1);
    EXPECT_EQ(utc_time.day, 15);
    EXPECT_EQ(utc_time.hour, 10);
    EXPECT_EQ(utc_time.minute, 0);
    EXPECT_EQ(utc_time.zone.sign, 'Z');
    EXPECT_EQ(utc_time.zone.hrs, 0);
    EXPECT_EQ(utc_time.zone.mins, 0);
}
