/**
 * \file test_counterExample2.cpp
 * \brief Tester exercising suite fixtures
 */
#include <gtest/gtest.h>

#include "counter.h"


struct testCounterEx2 : public ::testing::Test {
    /* Suite Fixture for counter_increment()
==============================================================================*/
    static void SetUpTestSuite()
    {
        counter_initialise();
        counter_increment();
        std::cout << "==========Set up done==========" << std::endl;
    }
    static void TearDownTestSuite()
    {
        counter_deInitialise();
        std::cout << "==========Tear down done==========" << std::endl;
    }
};


/* Functions that require setup in advance
==============================================================================*/
TEST_F(testCounterEx2, CounterSetUp)
{
    EXPECT_EQ(true, counter_isInitialised());
}

TEST_F(testCounterEx2, Count23)
{
    EXPECT_EQ(1, counter_get());
    counter_increment();
    EXPECT_EQ(2, counter_get());
    counter_increment();
    EXPECT_EQ(3, counter_get());
}
TEST_F(testCounterEx2, Count4)
{
    counter_increment();
    EXPECT_EQ(4, counter_get());
}


