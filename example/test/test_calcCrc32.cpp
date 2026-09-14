/**
 * \file test_crc32.cpp
 * \brief Tester exercising an algorithm
 */
#include <cstring>
#include <gtest/gtest.h>

#include "calcCrc32.h"

/*
 * test array results can be obtained here:
 * https://www.lammertbies.nl/comm/info/crc-calculation
 */

struct testCrc32 : public ::testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/* Algorithm
==============================================================================*/

// Test: Basic known vector
TEST_F(testCrc32, Calc)
{
    uint8_t u8TestBuffer[] = {0x5A, 0x5A, 0x5A, 0x5A};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0x2F359688);
}

// Test: NULL pointer should return 0
TEST_F(testCrc32, CalcNullPointer)
{
    ASSERT_EQ(calcCrc32(NULL, 100), 0x00000000);
}

// Test: Empty buffer should return seed (inverted)
TEST_F(testCrc32, CalcEmptyBuffer)
{
    uint8_t u8TestBuffer[] = {0};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], 0), 0x00000000);
}

// Test: Single byte
TEST_F(testCrc32, CalcSingleByte)
{
    uint8_t u8TestBuffer[] = {0x5A};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0x59BC5767);
}

// Test: All zeros
TEST_F(testCrc32, CalcAllZeros)
{
    uint8_t u8TestBuffer[8] = {0};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0x6522DF69);
}

// Test: All ones (0xFF)
TEST_F(testCrc32, CalcAllOnes)
{
    uint8_t u8TestBuffer[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0xFFFFFFFF);
}

// Test: Known test string "123456789"
TEST_F(testCrc32, CalcKnownString)
{
    uint8_t u8TestBuffer[] = {0x31, 0x32, 0x33, 0x34, 0x35,
                              0x36, 0x37, 0x38, 0x39};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0xCBF43926);
}

// Test: Large buffer
TEST_F(testCrc32, CalcLargeBuffer)
{
    uint8_t u8TestBuffer[256];
    memset(u8TestBuffer, 0xAA, sizeof(u8TestBuffer));
    uint32_t crc = calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer));
    // Verify it produces a consistent result
    ASSERT_EQ(crc, calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)));
}

// Test: Incremental data (0, 1, 2, 3, ...)
TEST_F(testCrc32, CalcIncrementalData)
{
    uint8_t u8TestBuffer[16];
    for ( int i = 0; i < 16; i++ ) {
        u8TestBuffer[i] = i;
    }
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0xCECEE288);
}

// Test: Alternating pattern (0x55, 0xAA, 0x55, 0xAA, ...)
TEST_F(testCrc32, CalcAlternatingPattern)
{
    uint8_t u8TestBuffer[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
    ASSERT_EQ(calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer)), 0x24187459);
}

// Test: Consistent results (deterministic)
TEST_F(testCrc32, CalcConsistent)
{
    uint8_t u8TestBuffer[] = {0x5A, 0x5A, 0x5A, 0x5A};
    uint32_t crc1          = calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer));
    uint32_t crc2          = calcCrc32(&u8TestBuffer[0], sizeof(u8TestBuffer));
    ASSERT_EQ(crc1, crc2);
}

// Test: Appending data changes CRC (sensitivity test)
TEST_F(testCrc32, CalcSensitivity)
{
    uint8_t u8TestBuffer1[] = {0x5A, 0x5A, 0x5A};
    uint8_t u8TestBuffer2[] = {0x5A, 0x5A, 0x5A, 0x5A};
    uint32_t crc1 = calcCrc32(&u8TestBuffer1[0], sizeof(u8TestBuffer1));
    uint32_t crc2 = calcCrc32(&u8TestBuffer2[0], sizeof(u8TestBuffer2));
    ASSERT_NE(crc1, crc2);
}
