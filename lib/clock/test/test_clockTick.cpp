#include <gtest/gtest.h>

#include "clockEvents.h"
#include "clockTick.h"
#include "timeDriver.h"
#include "utilDevDebug.h"

using namespace testing;

#define TST_ONE_MILLISECOND_US (uint32_t)(1000)
#define TST_ONE_SECOND_US      (uint32_t)(1000000)
#define TST_ONE_MINUTE_US      (uint32_t)(60 * TST_ONE_SECOND_US)
#define TST_ONE_HOUR_US        (uint32_t)(60 * TST_ONE_MINUTE_US)
#define TST_HR_IN_SECONDS      (uint32_t)(3600)

static uint32_t f_u32Counter;

uint32_t timeDriver_getAppTick_uS(void)
{
    return f_u32Counter;
}


struct testClk : public ::testing::Test {
    st_clock_tick_timestamp_t stCurrentTime;
    void increment_count(uint32_t i_u32Amount)
    {
        f_u32Counter += i_u32Amount;
        clockTick_getTimeStamp(&stCurrentTime);
    }
    virtual void SetUp() override
    {
        f_u32Counter               = 0;
        stCurrentTime.microseconds = 0;
        stCurrentTime.seconds      = 0;
        stCurrentTime.minutes      = 0;
        stCurrentTime.hours        = 0;
        clockTick_initialise();
        clockTick_getTimeStamp(&stCurrentTime);
        EXPECT_EQ(stCurrentTime.microseconds, 0);
        EXPECT_EQ(stCurrentTime.seconds, 0);
        EXPECT_EQ(stCurrentTime.minutes, 0);
        EXPECT_EQ(stCurrentTime.hours, 0);
    }
    virtual void TearDown() override {}
};

/* clockTick.h */
TEST_F(testClk, TimeIncrements)
{
    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);

    increment_count(500);

    EXPECT_EQ(stCurrentTime.microseconds, 500);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);
}


TEST_F(testClk, TicktimeUsUpdates)
{
    for ( uint32_t u32Time = 0; u32Time < TST_ONE_SECOND_US - 1; u32Time++ ) {

        EXPECT_EQ(stCurrentTime.microseconds, u32Time);
        increment_count(1);
    }
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);
}

TEST_F(testClk, TicktimeUpdatesToSecs)
{
    increment_count(TST_ONE_SECOND_US - 1);

    EXPECT_EQ(stCurrentTime.microseconds, 999999);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);

    increment_count(1);

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 1);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);

    increment_count(1);

    EXPECT_EQ(stCurrentTime.microseconds, 1);
    EXPECT_EQ(stCurrentTime.seconds, 1);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);
}

TEST_F(testClk, TicktimeSecondsUpdate)
{
    increment_count(TST_ONE_SECOND_US);

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 1);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);

    for ( uint32_t u32Time = 1; u32Time < 59; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 59);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 0);
}


TEST_F(testClk, TicktimeMinutesUpdate)
{
    for ( uint32_t u32Time = 0; u32Time < 60; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 1);
    EXPECT_EQ(stCurrentTime.hours, 0);

    for ( uint32_t u32Time = 0; u32Time < (58 * 60); u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 59);
    EXPECT_EQ(stCurrentTime.hours, 0);
}

TEST_F(testClk, TicktimeHoursUpdate)
{
    for ( uint32_t u32Time = 0; u32Time < 60 * 60; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }

    EXPECT_EQ(stCurrentTime.microseconds, 0);
    EXPECT_EQ(stCurrentTime.seconds, 0);
    EXPECT_EQ(stCurrentTime.minutes, 0);
    EXPECT_EQ(stCurrentTime.hours, 1);
}
TEST_F(testClk, HandlesWrapAround)
{
    /* 0xffffffff = 1 hr 11 min 34 s 965600 us */
    /* 1 hr */
    for ( uint32_t u32Time = 0; u32Time < 60 * 60; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }

    /* 11 min */
    for ( uint32_t u32Time = 0; u32Time < 11 * 60; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }
    /* 34 seconds */
    for ( uint32_t u32Time = 0; u32Time < 34; u32Time++ ) {
        increment_count(TST_ONE_SECOND_US);
    }
    /* 965600 us */
    for ( uint32_t u32Time = 0; u32Time < 9656; u32Time++ ) {
        increment_count(100);
    }
    /* max value */
    EXPECT_EQ(stCurrentTime.microseconds, 965600);
    EXPECT_EQ(stCurrentTime.seconds, 34);
    EXPECT_EQ(stCurrentTime.minutes, 11);
    EXPECT_EQ(stCurrentTime.hours, 1);

    increment_count(TST_ONE_SECOND_US);
    /* wrap around */
    EXPECT_EQ(stCurrentTime.microseconds, 965600);
    EXPECT_EQ(stCurrentTime.seconds, 35);
    EXPECT_EQ(stCurrentTime.minutes, 11);
    EXPECT_EQ(stCurrentTime.hours, 1);
}

TEST_F(testClk, ClockEventHasUsTimeElapsedZeroPeriodReturnsTrue)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    EXPECT_TRUE(clock_event_hasUsTimeElapsed(start_time, 0U));
}

TEST_F(testClk, ClockEventHasMsTimeElapsedZeroPeriodReturnsTrue)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    EXPECT_TRUE(clock_event_hasMsTimeElapsed(start_time, 0U));
}

TEST_F(testClk, ClockEventHasUsTimeElapsedFalseBeforeExpiryTrueAfter)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    increment_count(499U);
    EXPECT_FALSE(clock_event_hasUsTimeElapsed(start_time, 500U));

    increment_count(1U);
    EXPECT_TRUE(clock_event_hasUsTimeElapsed(start_time, 500U));
}

TEST_F(testClk, ClockEventHasUsTimeElapsedTrueWhenCurrentTimeGreaterInSeconds)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    increment_count(TST_ONE_SECOND_US * 2);
    EXPECT_TRUE(clock_event_hasUsTimeElapsed(start_time, TST_ONE_SECOND_US));
}

TEST_F(testClk, ClockEventHasUsTimeElapsedFalseWhenCurrentTimeBeforeExpire)
{
    st_clock_tick_timestamp_t start_time = {0U, 0U, 1U, 0U};

    EXPECT_FALSE(clock_event_hasUsTimeElapsed(start_time, 1000U));
}

TEST_F(testClk, ClockEventHasMsTimeElapsedUsesMilliseconds)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    increment_count(999U);
    EXPECT_FALSE(clock_event_hasMsTimeElapsed(start_time, 1U));

    increment_count(1U);
    EXPECT_TRUE(clock_event_hasMsTimeElapsed(start_time, 1U));
}

TEST_F(testClk, ClockEventStopWatchHandlesNullOutput)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;
    increment_count(1000U);

    EXPECT_NO_FATAL_FAILURE(clock_event_stopWatch(start_time, NULL));
}

TEST_F(testClk, ClockEventStopWatchHandlesLargeDeltaAcrossWrap)
{
    st_clock_tick_timestamp_t start_time = stCurrentTime;

    f_u32Counter = 0xffffffffU;
    clockTick_getTimeStamp(&stCurrentTime);
    EXPECT_EQ(stCurrentTime.hours, 1U);
    EXPECT_EQ(stCurrentTime.minutes, 11U);
    EXPECT_EQ(stCurrentTime.seconds, 34U);
    EXPECT_EQ(stCurrentTime.microseconds, 967295U);

    f_u32Counter                      = 0U;
    st_clock_tick_timestamp_t elapsed = {};
    clock_event_stopWatch(start_time, &elapsed);

    EXPECT_EQ(elapsed.hours, 1U);
    EXPECT_EQ(elapsed.minutes, 11U);
    EXPECT_EQ(elapsed.seconds, 34U);
    EXPECT_EQ(elapsed.microseconds, 967296U);
}

struct testClkStr : public ::testing::Test {
    char Result[24];
    st_clock_tick_timestamp_t stTestTime;
    virtual void SetUp() override
    {
        stTestTime.microseconds = 0;
        stTestTime.seconds      = 0;
        stTestTime.minutes      = 0;
        stTestTime.hours        = 0;
    }
    virtual void TearDown() override {}
};

TEST_F(testClkStr, ToString0)
{
    EXPECT_EQ(1, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("0"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}

TEST_F(testClkStr, ToString1microsecond)
{

    stTestTime.microseconds = 1;
    EXPECT_EQ(1, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("1"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToString1second)
{

    stTestTime.seconds = 1;
    EXPECT_EQ(3, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("1:0"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToString1minute)
{

    stTestTime.minutes = 1;
    EXPECT_EQ(5, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("1:0:0"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToString1hour)
{

    stTestTime.hours = 1;
    EXPECT_EQ(7, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("1:0:0:0"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToStringFull)
{

    stTestTime.microseconds = 123456;
    stTestTime.seconds      = 59;
    stTestTime.minutes      = 59;
    stTestTime.hours        = 23;
    EXPECT_EQ(15, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("23:59:59:123456"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToStringLotsOfHours)
{

    stTestTime.microseconds = 123456;
    stTestTime.seconds      = 59;
    stTestTime.minutes      = 59;
    stTestTime.hours        = 12345;
    EXPECT_EQ(18, clockTick_getTimeString(stTestTime, Result, 24));
    EXPECT_EQ(std::string("12345:59:59:123456"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}
TEST_F(testClkStr, ToStringTruncates)
{

    stTestTime.microseconds = 123456;
    stTestTime.seconds      = 59;
    stTestTime.minutes      = 59;
    stTestTime.hours        = 23;
    EXPECT_EQ(12, clockTick_getTimeString(stTestTime, Result, 13));
    EXPECT_EQ(std::string("23:59:59:123"),
              std::string(reinterpret_cast<const char*>(&Result[0])));
}

/* clock_event.h */

TEST_F(testClk, EventUsElapsed)
{
    st_clock_tick_timestamp_t stStartTime;
    stStartTime.microseconds = 123456;
    stStartTime.seconds      = 1;
    stStartTime.minutes      = 1;
    stStartTime.hours        = 0;

    increment_count(TST_ONE_MINUTE_US);
    increment_count(TST_ONE_SECOND_US);
    increment_count(123456);

    for ( uint32_t u32Us = 1; u32Us < 1234567; u32Us++ ) {
        EXPECT_EQ(false, clock_event_hasUsTimeElapsed(stStartTime, u32Us));
        increment_count(1);
        EXPECT_EQ(true, clock_event_hasUsTimeElapsed(stStartTime, u32Us));
    }
}

TEST_F(testClk, EventMsElapsed)
{
    st_clock_tick_timestamp_t stStartTime;

    stStartTime.microseconds = 0;
    stStartTime.seconds      = 1;
    stStartTime.minutes      = 0;
    stStartTime.hours        = 0;

    increment_count(
        TST_ONE_SECOND_US); /* start and current are both 1 second */

    /* 1ms - 999ms */
    for ( uint32_t u32Ms = 1; u32Ms < 999; u32Ms++ ) {
        increment_count(TST_ONE_MILLISECOND_US - 1);
        EXPECT_EQ(false,
                  clock_event_hasMsTimeElapsed(stStartTime, (u32Ms * 1)));
        increment_count(1);
        EXPECT_EQ(true, clock_event_hasMsTimeElapsed(stStartTime, (u32Ms * 1)));
    }
}

TEST_F(testClk, EventSecondsElapsed)
{
    st_clock_tick_timestamp_t stStartTime;

    stStartTime.microseconds = 0;
    stStartTime.seconds      = 1;
    stStartTime.minutes      = 0;
    stStartTime.hours        = 0;

    increment_count(
        TST_ONE_SECOND_US); /* start and current are both 1 second */

    /* 1s - 59s */
    for ( uint32_t u32Ms = 1; u32Ms < 59; u32Ms++ ) {
        increment_count(TST_ONE_SECOND_US - 1);
        EXPECT_EQ(false,
                  clock_event_hasMsTimeElapsed(stStartTime, (u32Ms * 1000)));
        increment_count(1);
        EXPECT_EQ(true,
                  clock_event_hasMsTimeElapsed(stStartTime, (u32Ms * 1000)));
    }
}

TEST_F(testClk, EventMinutesElapsed)
{
    st_clock_tick_timestamp_t stStartTime;

    stStartTime.microseconds = 0;
    stStartTime.seconds      = 1;
    stStartTime.minutes      = 0;
    stStartTime.hours        = 0;

    increment_count(
        TST_ONE_SECOND_US); /* start and current are both 1 second */

    /* 1m - 59m */
    for ( uint32_t u32Ms = 1; u32Ms < 59; u32Ms++ ) {
        increment_count((TST_ONE_MINUTE_US - 1));
        EXPECT_EQ(false, clock_event_hasMsTimeElapsed(stStartTime,
                                                      (u32Ms * 1000 * 60)));
        increment_count(1);
        EXPECT_EQ(true, clock_event_hasMsTimeElapsed(stStartTime,
                                                     (u32Ms * 1000 * 60)));
    }
}
TEST_F(testClk, EventHrElapsed)
{
    st_clock_tick_timestamp_t stStartTime;
    uint32_t u32HrinMs       = 1000 * 60 * 60;
    stStartTime.microseconds = 0;
    stStartTime.seconds      = 1;
    stStartTime.minutes      = 0;
    stStartTime.hours        = 0;

    increment_count(
        TST_ONE_SECOND_US); /* start and current are both 1 second */

    /* 1 hr - 1193 hrs , passes
    for ( uint32_t u32Ms = 1; u32Ms < 1194; u32Ms++ ) {*/
    for ( uint32_t u32Ms = 1; u32Ms < 24; u32Ms++ ) {
        increment_count((TST_ONE_HOUR_US - 1));
        EXPECT_EQ(false, clock_event_hasMsTimeElapsed(stStartTime,
                                                      (u32Ms * u32HrinMs)));
        increment_count(1);
        EXPECT_EQ(true, clock_event_hasMsTimeElapsed(stStartTime,
                                                     (u32Ms * u32HrinMs)));
    }
}

TEST_F(testClk, StopWatchUs)
{
    st_clock_tick_timestamp_t stStartTime;
    st_clock_tick_timestamp_t tResult;
    st_clock_tick_timestamp_t tExpected;

    /* 00:40:50:607080 */
    stStartTime.hours        = 0;
    stStartTime.minutes      = 40;
    stStartTime.seconds      = 50;
    stStartTime.microseconds = 607080;

    /* FW current time to go beyond the start time  */
    increment_count((TST_ONE_MINUTE_US * 40));
    increment_count((TST_ONE_SECOND_US * 50));
    increment_count(708090);

    tExpected.hours        = 0;
    tExpected.minutes      = 0;
    tExpected.seconds      = 0;
    tExpected.microseconds = 101010;

    clock_event_stopWatch(stStartTime, &tResult);
    EXPECT_EQ(tResult.hours, tExpected.hours);
    EXPECT_EQ(tResult.minutes, tExpected.minutes);
    EXPECT_EQ(tResult.seconds, tExpected.seconds);
    EXPECT_EQ(tResult.microseconds, tExpected.microseconds);
}

TEST_F(testClk, StopWatchSeconds)
{
    st_clock_tick_timestamp_t stStartTime;
    st_clock_tick_timestamp_t tResult;
    st_clock_tick_timestamp_t tExpected;

    /* 00:40:50:607080 */
    stStartTime.hours        = 0;
    stStartTime.minutes      = 40;
    stStartTime.seconds      = 50;
    stStartTime.microseconds = 607080;

    /* FW current time to go beyond the start time  */
    increment_count((TST_ONE_MINUTE_US * 40));
    increment_count((TST_ONE_SECOND_US * 59));
    increment_count(708090);

    tExpected.hours        = 0;
    tExpected.minutes      = 0;
    tExpected.seconds      = 9;
    tExpected.microseconds = 101010;

    clock_event_stopWatch(stStartTime, &tResult);
    EXPECT_EQ(tResult.hours, tExpected.hours);
    EXPECT_EQ(tResult.minutes, tExpected.minutes);
    EXPECT_EQ(tResult.seconds, tExpected.seconds);
    EXPECT_EQ(tResult.microseconds, tExpected.microseconds);
}

TEST_F(testClk, StopWatchMin)
{
    st_clock_tick_timestamp_t stStartTime;
    st_clock_tick_timestamp_t tResult;
    st_clock_tick_timestamp_t tExpected;

    /* 00:40:50:607080 */
    stStartTime.hours        = 0;
    stStartTime.minutes      = 40;
    stStartTime.seconds      = 50;
    stStartTime.microseconds = 607080;

    /* FW current time to go beyond the start time  */
    increment_count((TST_ONE_MINUTE_US * 45));
    increment_count((TST_ONE_SECOND_US * 59));
    increment_count(708090);

    tExpected.hours        = 0;
    tExpected.minutes      = 5;
    tExpected.seconds      = 9;
    tExpected.microseconds = 101010;

    clock_event_stopWatch(stStartTime, &tResult);
    EXPECT_EQ(tResult.hours, tExpected.hours);
    EXPECT_EQ(tResult.minutes, tExpected.minutes);
    EXPECT_EQ(tResult.seconds, tExpected.seconds);
    EXPECT_EQ(tResult.microseconds, tExpected.microseconds);
}

TEST_F(testClk, StopWatchHr)
{
    st_clock_tick_timestamp_t stStartTime;
    st_clock_tick_timestamp_t tResult;
    st_clock_tick_timestamp_t tExpected;

    /* 00:40:50:607080 */
    stStartTime.hours        = 0;
    stStartTime.minutes      = 40;
    stStartTime.seconds      = 50;
    stStartTime.microseconds = 607080;

    /* FW current time to go beyond the start time  */
    increment_count((TST_ONE_HOUR_US));
    increment_count((TST_ONE_MINUTE_US * 45));
    increment_count((TST_ONE_SECOND_US * 59));
    increment_count(708090);

    tExpected.hours        = 1;
    tExpected.minutes      = 5;
    tExpected.seconds      = 9;
    tExpected.microseconds = 101010;

    clock_event_stopWatch(stStartTime, &tResult);
    EXPECT_EQ(tResult.hours, tExpected.hours);
    EXPECT_EQ(tResult.minutes, tExpected.minutes);
    EXPECT_EQ(tResult.seconds, tExpected.seconds);
    EXPECT_EQ(tResult.microseconds, tExpected.microseconds);
}
