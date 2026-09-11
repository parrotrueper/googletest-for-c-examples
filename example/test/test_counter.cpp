/**
 * \file test_counter.cpp
 * \brief Tester exercising test setup and tear down
 */
#include <gtest/gtest.h>

#include "counter.h"


struct testCounter : public ::testing::Test {
    /* Fixture for counter_increment()
==============================================================================*/
    virtual void SetUp() override
    {
        counter_initialise();
        std::cout << "==========Set up done==========" << std::endl;
    }
    virtual void TearDown() override
    {
        counter_deInitialise();
        std::cout << "==========Tear down done==========" << std::endl;
    }
};


/* Functions that require setup in advance
==============================================================================*/
TEST_F(testCounter, CounterSetUp)
{
    EXPECT_EQ(true, counter_isInitialised());
}

TEST_F(testCounter, Count123)
{
    EXPECT_EQ(0, counter_get());
    counter_increment();
    EXPECT_EQ(1, counter_get());
    counter_increment();
    EXPECT_EQ(2, counter_get());
}

TEST_F(testCounter, Count1)
{
    counter_increment();
    EXPECT_EQ(1, counter_get());
}


