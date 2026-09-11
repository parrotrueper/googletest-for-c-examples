/**
 * \file test_utilCrc32.cpp
 * \brief Tester exercising an algorithm
 */
#include <gtest/gtest.h>

#include "utilCrc32.h"

/* test array results can be obtained here:
https://www.lammertbies.nl/comm/info/crc-calculation
*/

struct util_Crc32_test : public ::testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/* Algorithm
==============================================================================*/
TEST_F(util_Crc32_test, Calc)
{
    uint8_t u8TestBuffer[] = {0x5A, 0x5A, 0x5A, 0x5A};

    ASSERT_EQ(
        util_Crc32_Calc(&u8TestBuffer[0], (uint32_t)(sizeof(u8TestBuffer))),
        0x2F359688);
}
