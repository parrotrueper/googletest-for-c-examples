/**
 * \file test_evaluate.cpp
 * \brief Tester exercising floating point comparisons,
 * bitwise comparisons, disabling tests, skipping tests,
 * death tests
 */
#include <bitset>
#include <gtest/gtest.h>
#include <iomanip>

#include "evaluate.h"

static bool bDebugEn = true; /* check prints work */

struct testEvaluate : public ::testing::Test {
    virtual void SetUp() override {}
    virtual void TearDown() override {}
};

TEST_F(testEvaluate, PlatformSizeOfFloat)
{
    EXPECT_EQ(32, evaluate_platformSizeOfFloat());
}

TEST_F(testEvaluate, PlatformSizeOfDouble)
{
    EXPECT_EQ(64, evaluate_platformSizeOfDouble());
}

TEST_F(testEvaluate, PlatformSizeOfLongDouble)
{
    EXPECT_EQ(128, evaluate_platformSizeOfLongDouble());
}

/* compare 2 numbers bitwise
==============================================================================*/
TEST_F(testEvaluate, DoTheseBitsMatch)
{
    uint8_t u8Result   = 0x55;
    uint8_t u8Expected = 0x5A;

    std::bitset<8> bitResult   = std::bitset<8>(u8Result);
    std::bitset<8> bitExpected = std::bitset<8>(u8Expected);


    EXPECT_NE(bitExpected, bitResult);
}

/* checking floating point numbers
==============================================================================*/
TEST_F(testEvaluate, GetPi32bit)
{
    if ( bDebugEn ) {
        std::cout << std::setprecision(31) << evaluate_getPi32bit()
                  << std::endl;
    }
    EXPECT_FLOAT_EQ(evaluate_getPi32bit(), 3.1415926535897966);
}

TEST_F(testEvaluate, GetPi64bit)
{
    if ( bDebugEn ) {
        std::cout << std::setprecision(63) << evaluate_getPi64bit()
                  << std::endl;
    }
    EXPECT_DOUBLE_EQ(evaluate_getPi64bit(),
                     3.14159265358979311599796346854418516);
}
/* check floating point numbers with an acceptable error
==============================================================================*/
TEST_F(testEvaluate, Pi32bitErrorTolerancePass)
{
    if ( bDebugEn ) {
        std::cout << std::setprecision(31) << evaluate_getPi32bit()
                  << std::endl;
    }
    EXPECT_NEAR(evaluate_getPi32bit(), 3.1415927410125731, 0.0000000000000001);
}

TEST_F(testEvaluate, Pi64bitErrorTolerancePass)
{
    if ( bDebugEn ) {
        std::cout << std::setprecision(63) << evaluate_getPi64bit()
                  << std::endl;
    }
    EXPECT_NEAR(evaluate_getPi64bit(), 3.1415926535897932, 0.0000000000000001);
}

/* Disabling tests that fail while you fix your bug
==============================================================================*/
TEST_F(testEvaluate, DISABLED_Pi32bitErrorToleranceFail)
{ /* to re-enable remove "DISABLED_" */
    if ( bDebugEn ) {
        std::cout << std::setprecision(31) << evaluate_getPi32bit()
                  << std::endl;
    }
    EXPECT_NEAR(evaluate_getPi32bit(), 3.1415926535897935, 0.0000000000000001);
}

TEST_F(testEvaluate, Pi64bitErrorToleranceFail)
{
    GTEST_SKIP() << "This is how to SKIP a test, can also be used in a "
                    "fixture."; /* to re-enable remove this line */
    if ( bDebugEn ) {
        std::cout << std::setprecision(63) << evaluate_getPi64bit()
                  << std::endl;
    }
    EXPECT_NEAR(evaluate_getPi64bit(), 3.1415926535897935, 0.0000000000000001);
}

/* Function abuse that will cause a segmentation fault
=============================================================================*/
TEST_F(testEvaluate, SegmentationFault)
{
    uint8_t u8Dummy = 0x55;
    EXPECT_EQ("0x55", std::string(reinterpret_cast<const char*>(
                          evaluate_showAddress(&u8Dummy))));
    EXPECT_DEATH(std::string(evaluate_showAddress(0)), "");
}

/* Function abuse that will cause a core dump
=============================================================================*/
TEST_F(testEvaluate, CoreDump)
{
    EXPECT_EQ(2, evaluate_divide(2, 1));
    EXPECT_DEATH(evaluate_divide(1, 0), "");
}
