#include <gtest/gtest.h>

#include "dateTime.h"

using namespace testing;

/* isLeapYear */
class isLeapYearTestFixture : public TestWithParam<std::tuple<uint16_t, bool>>
{
};
TEST_P(isLeapYearTestFixture, isLeapYearTest)
{
    uint16_t year = std::get<0>(GetParam());
    bool expected = std::get<1>(GetParam());
    bool actual   = dateTime_isLeapYear(year);
    EXPECT_EQ(actual, expected);
}
INSTANTIATE_TEST_SUITE_P(
    isLeapYearTests, isLeapYearTestFixture,
    Values(std::make_tuple(2020, true),  /* leap year */
           std::make_tuple(1993, false), /* not a leap year */
           std::make_tuple(1992, true),  /* leap year */
           std::make_tuple(1900, false), /* not a leap year */
           std::make_tuple(2021, false), /* not a leap year */
           std::make_tuple(2024, true),  /* leap year */
           std::make_tuple(1900, false), /* not a leap year */
           std::make_tuple(2000, true),  /* leap year */
           std::make_tuple(2100, false), /* not a leap year */
           std::make_tuple(2400, true),  /* leap year */
           std::make_tuple(2500, false) /* not a leap year */));

/* days of the month */
class daysInMonthTestFixture
    : public TestWithParam<std::tuple<uint16_t, uint8_t, uint8_t>>
{
};
TEST_P(daysInMonthTestFixture, daysInMonthTest)
{
    uint16_t year         = std::get<0>(GetParam());
    uint8_t month         = std::get<1>(GetParam());
    uint8_t expected_days = std::get<2>(GetParam());
    uint8_t actual_days   = dateTime_getDaysInMonth(year, month);
    EXPECT_EQ(actual_days, expected_days);
}
INSTANTIATE_TEST_SUITE_P(
    daysInMonthTests, daysInMonthTestFixture,
    Values(std::make_tuple(2020, 1, 31),  /* Jan */
           std::make_tuple(2020, 2, 29),  /* Feb leap year */
           std::make_tuple(2021, 2, 28),  /* Feb not leap year */
           std::make_tuple(2020, 3, 31),  /* Mar */
           std::make_tuple(2020, 4, 30),  /* Apr */
           std::make_tuple(2020, 5, 31),  /* May */
           std::make_tuple(2020, 6, 30),  /* Jun */
           std::make_tuple(2020, 7, 31),  /* Jul */
           std::make_tuple(2020, 8, 31),  /* Aug */
           std::make_tuple(2020, 9, 30),  /* Sep */
           std::make_tuple(2020, 10, 31), /* Oct */
           std::make_tuple(2020, 11, 30), /* Nov */
           std::make_tuple(2020, 12, 31) /* Dec */));

/* valid date */
class validDateTestFixture
    : public TestWithParam<std::tuple<uint16_t, uint8_t, uint8_t, bool>>
{
};
TEST_P(validDateTestFixture, validDateTest)
{
    uint16_t year = std::get<0>(GetParam());
    uint8_t month = std::get<1>(GetParam());
    uint8_t day   = std::get<2>(GetParam());
    bool expected = std::get<3>(GetParam());
    bool actual   = dateTime_isValidDate(year, month, day);
    EXPECT_EQ(actual, expected);
}
INSTANTIATE_TEST_SUITE_P(
    validDateTests, validDateTestFixture,
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

/* valid time */
class validTimeTestFixture
    : public TestWithParam<
          std::tuple<uint8_t, uint8_t, uint8_t, uint32_t, bool>>
{
};
TEST_P(validTimeTestFixture, validTimeTest)
{
    uint8_t hour          = std::get<0>(GetParam());
    uint8_t minute        = std::get<1>(GetParam());
    uint8_t second        = std::get<2>(GetParam());
    uint32_t microseconds = std::get<3>(GetParam());
    bool expected         = std::get<4>(GetParam());
    bool actual = dateTime_isValidTime(hour, minute, second, microseconds);
    EXPECT_EQ(actual, expected);
}
INSTANTIATE_TEST_SUITE_P(
    validTimeTests, validTimeTestFixture,
    Values(std::make_tuple(0, 0, 0, 0, true),         /* minimum valid time */
           std::make_tuple(23, 59, 59, 999999, true), /* maximum valid time */
           std::make_tuple(24, 0, 0, 0, false),       /* hour out of bounds */
           std::make_tuple(0, 60, 0, 0, false),       /* minute out of bounds */
           std::make_tuple(0, 0, 60, 0, false),       /* second out of bounds */
           std::make_tuple(0, 0, 0, 1000000,
                           false) /* microseconds out of bounds */
           ));

/* combined valid date time */
class validDateTimeTestFixture
    : public TestWithParam<std::tuple<uint16_t, uint8_t, uint8_t, uint8_t,
                                      uint8_t, uint8_t, uint32_t, bool>>
{
};
TEST_P(validDateTimeTestFixture, validDateTimeTest)
{
    uint16_t year         = std::get<0>(GetParam());
    uint8_t month         = std::get<1>(GetParam());
    uint8_t day           = std::get<2>(GetParam());
    uint8_t hour          = std::get<3>(GetParam());
    uint8_t minute        = std::get<4>(GetParam());
    uint8_t second        = std::get<5>(GetParam());
    uint32_t microseconds = std::get<6>(GetParam());
    bool expected         = std::get<7>(GetParam());
    bool actual = dateTime_isValidDateTime(year, month, day, hour, minute,
                                           second, microseconds);
    EXPECT_EQ(actual, expected);
}
INSTANTIATE_TEST_SUITE_P(
    validDateTimeTests, validDateTimeTestFixture,
    Values(std::make_tuple(2020, 1, 31, 23, 59, 59, 999999, true), /* valid */
           std::make_tuple(2020, 2, 29, 12, 0, 0, 0, true),        /* valid */
           std::make_tuple(2021, 2, 29, 12, 0, 0, 0, false),       /* invalid */
           std::make_tuple(2020, 4, 31, 12, 0, 0, 0, false),       /* invalid */
           std::make_tuple(2020, 1, 31, 24, 0, 0, 0, false),       /* invalid */
           std::make_tuple(2020, 1, 31, 23, 60, 0, 0, false),      /* invalid */
           std::make_tuple(2020, 1, 31, 23, 59, 60, 0, false),     /* invalid */
           std::make_tuple(2020, 1, 31, 23, 59, 59, 1000000,
                           false), /* invalid */
                                   /* valid and invalid leap years */
           std::make_tuple(2020, 2, 29, 12, 0, 0, 0, true),  /* valid */
           std::make_tuple(2021, 2, 29, 12, 0, 0, 0, false), /* invalid */
           std::make_tuple(2024, 2, 29, 12, 0, 0, 0, true),  /* valid */
           std::make_tuple(2025, 2, 29, 12, 0, 0, 0, false), /* invalid */
           std::make_tuple(1900, 2, 29, 12, 0, 0, 0, false), /* invalid */
           std::make_tuple(2000, 2, 29, 12, 0, 0, 0, true),  /* valid */
           std::make_tuple(2100, 2, 29, 12, 0, 0, 0, false), /* invalid */
           std::make_tuple(2400, 2, 29, 12, 0, 0, 0, true),  /* valid */
           std::make_tuple(2500, 2, 29, 12, 0, 0, 0, false)  /* invalid */
           ));
