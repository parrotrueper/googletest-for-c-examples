#include <gtest/gtest.h>

#include "dateTime.h"
#include "utcNorm.h"

using namespace testing;

/* all valid timezones */
static st_utc_zone_t valid_timezones[] = {
    {'Z', 0, 0},  {'-', 12, 0},  {'-', 11, 0}, {'-', 10, 0},  {'-', 9, 30},
    {'-', 9, 0},  {'-', 8, 0},   {'-', 7, 0},  {'-', 6, 0},   {'-', 5, 0},
    {'-', 4, 0},  {'-', 3, 30},  {'-', 3, 0},  {'-', 2, 0},   {'-', 1, 0},
    {'+', 0, 0},  {'+', 1, 0},   {'+', 2, 0},  {'+', 3, 0},   {'+', 3, 30},
    {'+', 4, 0},  {'+', 4, 30},  {'+', 5, 0},  {'+', 5, 30},  {'+', 5, 45},
    {'+', 6, 0},  {'+', 6, 30},  {'+', 7, 0},  {'+', 8, 0},   {'+', 8, 45},
    {'+', 9, 0},  {'+', 9, 30},  {'+', 10, 0}, {'+', 10, 30}, {'+', 11, 0},
    {'+', 12, 0}, {'+', 12, 45}, {'+', 13, 0}, {'+', 14, 0}};

static st_utc_time_t utcNorm_expectedUtcZ(st_utc_time_t local)
{
    int32_t offset_minutes = (int32_t)local.zone.hrs * 60 + local.zone.mins;
    if ( local.zone.sign == '-' ) {
        offset_minutes = -offset_minutes;
    }

    int32_t total_minutes = (int32_t)local.hour * 60 + local.minute
                            - offset_minutes;
    int32_t day_delta = 0;
    while ( total_minutes < 0 ) {
        total_minutes += 1440;
        day_delta--;
    }
    while ( total_minutes >= 1440 ) {
        total_minutes -= 1440;
        day_delta++;
    }

    local.hour   = (uint8_t)(total_minutes / 60);
    local.minute = (uint8_t)(total_minutes % 60);

    if ( day_delta != 0 ) {
        int32_t day   = (int32_t)local.day + day_delta;
        int32_t month = local.month;
        int32_t year  = local.year;

        while ( day < 1 ) {
            if ( month > 1 ) {
                month--;
            } else {
                month = 12;
                year--;
            }
            day += dateTime_getDaysInMonth((uint16_t)year, (uint8_t)month);
        }
        while ( day
                > dateTime_getDaysInMonth((uint16_t)year, (uint8_t)month) ) {
            day -= dateTime_getDaysInMonth((uint16_t)year, (uint8_t)month);
            month++;
            if ( month > 12 ) {
                month = 1;
                year++;
            }
        }

        local.year  = (uint16_t)year;
        local.month = (uint8_t)month;
        local.day   = (uint8_t)day;
    }

    local.zone.sign = 'Z';
    local.zone.hrs  = 0;
    local.zone.mins = 0;
    return local;
}

TEST(UtcNormToUtcZTest, normalisesAllValidTimezones)
{
    st_utc_time_t local = {2024, 2, 29, 12, 34, 56, 123456, {'Z', 0, 0}};

    for ( const st_utc_zone_t& zone : valid_timezones ) {
        st_utc_time_t input = local;
        input.zone          = zone;

        st_utc_time_t expected = utcNorm_expectedUtcZ(input);
        utcNorm_toUtcZ(&input);

        EXPECT_EQ(input.year, expected.year);
        EXPECT_EQ(input.month, expected.month);
        EXPECT_EQ(input.day, expected.day);
        EXPECT_EQ(input.hour, expected.hour);
        EXPECT_EQ(input.minute, expected.minute);
        EXPECT_EQ(input.second, expected.second);
        EXPECT_EQ(input.microseconds, expected.microseconds);
        EXPECT_EQ(input.zone.sign, expected.zone.sign);
        EXPECT_EQ(input.zone.hrs, expected.zone.hrs);
        EXPECT_EQ(input.zone.mins, expected.zone.mins);
    }
}

/* -- normalise to UTC +00:00 -- */
class utcNormToUtcZTestFixture
    : public testing::TestWithParam<std::tuple<st_utc_time_t, st_utc_time_t>>
{
};
TEST_P(utcNormToUtcZTestFixture, utcNormToUtcZTest)
{
    st_utc_time_t input    = std::get<0>(GetParam());
    st_utc_time_t expected = std::get<1>(GetParam());

    utcNorm_toUtcZ(&input);
    EXPECT_EQ(input.year, expected.year);
    EXPECT_EQ(input.month, expected.month);
    EXPECT_EQ(input.day, expected.day);
    EXPECT_EQ(input.hour, expected.hour);
    EXPECT_EQ(input.minute, expected.minute);
    EXPECT_EQ(input.second, expected.second);
    EXPECT_EQ(input.microseconds, expected.microseconds);
    EXPECT_EQ(input.zone.sign, expected.zone.sign);
    EXPECT_EQ(input.zone.hrs, expected.zone.hrs);
    EXPECT_EQ(input.zone.mins, expected.zone.mins);
}

INSTANTIATE_TEST_SUITE_P(
    utcNormToUtcZTests, utcNormToUtcZTestFixture,
    Values(
        std::make_tuple(st_utc_time_t{2024, 2, 29, 12, 0, 0, 0, {'Z', 0, 0}},
                        st_utc_time_t{2024, 2, 29, 12, 0, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 2, 29, 12, 0, 0, 0, {'+', 2, 0}},
                        st_utc_time_t{2024, 2, 29, 10, 0, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 2, 29, 12, 0, 0, 0, {'-', 5, 30}},
                        st_utc_time_t{2024, 2, 29, 17, 30, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 3, 1, 1, 30, 0, 0, {'+', 14, 0}},
                        st_utc_time_t{2024, 2, 29, 11, 30, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 1, 1, 1, 0, 0, 0, {'+', 14, 0}},
                        st_utc_time_t{2023, 12, 31, 11, 0, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2023, 12, 31, 23, 30, 0, 0, {'-', 12, 0}},
                        st_utc_time_t{2024, 1, 1, 11, 30, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 2, 28, 21, 15, 0, 0, {'-', 5, 45}},
                        st_utc_time_t{2024, 2, 29, 3, 0, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 1, 31, 23, 30, 0, 0, {'-', 8, 0}},
                        st_utc_time_t{2024, 2, 1, 7, 30, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(st_utc_time_t{2024, 12, 31, 23, 30, 0, 0, {'-', 11, 0}},
                        st_utc_time_t{2025, 1, 1, 10, 30, 0, 0, {'Z', 0, 0}}),
        std::make_tuple(
            st_utc_time_t{2024, 3, 1, 0, 15, 0, 0, {'+', 5, 45}},
            st_utc_time_t{2024, 2, 29, 18, 30, 0, 0, {'Z', 0, 0}})));
