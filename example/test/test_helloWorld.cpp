/**
 * \file test_helloWorld.cpp
 * \brief Tester exercising API printing to stdout, string checks,
 * and various C array comparisons
 */
#include <gmock/gmock.h>
#include <string.h>

#include "helloWorld.h"
#include "utilUnitTestWrapper.h"

using namespace std;
using namespace testing;

struct testHelloWorld : public ::testing::Test {
    virtual void SetUp() override {}
    virtual void TearDown() override {}
};

/* Misc tests
==============================================================================*/
TEST_F(testHelloWorld, PassNullPointer)
{
    EXPECTING_ASSERTS(2);

    bool bResult = helloWorld_main(0, 1);

    EXPECT_EQ(false, bResult);
    EXPECT_TRUE(DID_ASSERT());
    EXPECTING_ASSERTS(0);
}

TEST_F(testHelloWorld, PassZeroLength)
{
    EXPECTING_ASSERTS(2);

    uint8_t u8VirtualScreen;
    bool bResult = helloWorld_main(&u8VirtualScreen, 0);

    EXPECT_EQ(false, bResult);
    EXPECT_TRUE(DID_ASSERT());
    EXPECTING_ASSERTS(0);
}

TEST_F(testHelloWorld, ScreenSizeOne)
{

    uint8_t u8VirtualScreen;
    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen, sizeof(u8VirtualScreen));

    EXPECT_EQ(false, bResult);
    EXPECT_EQ(u8VirtualScreen, 0);
}

TEST_F(testHelloWorld, ScreenTooSmall)
{

    uint8_t u8VirtualScreen[6];
    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));

    EXPECT_EQ(false, bResult);
    EXPECT_EQ(string("Hello"),
              string(reinterpret_cast<const char*>(&u8VirtualScreen[0])));
}

TEST_F(testHelloWorld, ScreenEnoughSize)
{

    uint8_t u8VirtualScreen[13];
    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));

    EXPECT_EQ(true, bResult);
    EXPECT_EQ(string("Hello World!"),
              string(reinterpret_cast<const char*>(&u8VirtualScreen[0])));
}

/* Array comparison tests
==============================================================================*/
TEST_F(testHelloWorld, ScreenNotBlank)
{
    uint8_t u8VirtualScreen[23];
    uint8_t u8BlankScreen[23] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);

    EXPECT_EQ(sizeof(u8VirtualScreen), sizeof(u8BlankScreen));
    for ( uint32_t u32Ix = 0;
          u32Ix < strlen(reinterpret_cast<const char*>(&u8VirtualScreen[0]));
          u32Ix++ ) {
        EXPECT_NE(u8VirtualScreen[u32Ix], u8BlankScreen[u32Ix])
            << "Vectors x and y are equal at index " << u32Ix;
    }
}

TEST_F(testHelloWorld, ScreenMatch)
{

    uint8_t u8VirtualScreen[23];
    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));

    EXPECT_EQ(true, bResult);
    EXPECT_EQ(string("Hello World!"),
              string(reinterpret_cast<const char*>(&u8VirtualScreen[0])));
}

/* array comparison examples */
TEST_F(testHelloWorld, ArrayCmpEg1)
{

    uint8_t u8VirtualScreen[23];

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);

    /* Eg 1 */
    EXPECT_THAT(u8VirtualScreen,
                ElementsAre('H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l',
                            'd', '!', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST_F(testHelloWorld, ArrayCmpEg2)
{

    uint8_t u8VirtualScreen[23];
    uint8_t u8ReferenceScreen[23] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o',
                                     'r', 'l', 'd', '!', 0,   0,   0,   0,
                                     0,   0,   0,   0,   0,   0};

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);
    /* Eg 2 */
    EXPECT_EQ(sizeof(u8VirtualScreen), sizeof(u8ReferenceScreen));
    for ( uint32_t u32Ix = 0; u32Ix < sizeof(u8VirtualScreen); u32Ix++ ) {
        EXPECT_EQ(u8VirtualScreen[u32Ix], u8ReferenceScreen[u32Ix])
            << "Vectors x and y differ at index " << u32Ix;
    }
}

TEST_F(testHelloWorld, ArrayCmpEg3)
{

    uint8_t u8VirtualScreen[23];
    uint8_t u8ReferenceScreen[23] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o',
                                     'r', 'l', 'd', '!', 0,   0,   0,   0,
                                     0,   0,   0,   0,   0,   0};

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);
    /* Eg 3 */
    EXPECT_TRUE(0
                == memcmp(u8VirtualScreen, u8ReferenceScreen,
                          sizeof(u8ReferenceScreen)));
}

TEST_F(testHelloWorld, ArrayCmpEg4)
{

    uint8_t u8VirtualScreen[23];
    uint8_t u8ReferenceScreen[23] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o',
                                     'r', 'l', 'd', '!', 0,   0,   0,   0,
                                     0,   0,   0,   0,   0,   0};

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);

    /* Eg 4 */
    {
        auto pExpected = reinterpret_cast<
            std::array<uint8_t, sizeof(u8ReferenceScreen)>*>(u8ReferenceScreen);
        auto pActual = reinterpret_cast<
            const std::array<uint8_t, sizeof(u8VirtualScreen)>*>(
            u8VirtualScreen);
        ASSERT_EQ(*pExpected, *pActual);
    }
}

/* The easiest way to compare C arrays
==============================================================================*/
TEST_F(testHelloWorld, ArrayCmpEg5)
{

    uint8_t u8VirtualScreen[23];
    uint8_t u8ReferenceScreen[23] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o',
                                     'r', 'l', 'd', '!', 0,   0,   0,   0,
                                     0,   0,   0,   0,   0,   0};

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);

    /* Eg 5 */
    ASSERT_THAT(u8VirtualScreen, ContainerEq(u8ReferenceScreen));
}
/* same as above, but designed to fail
TEST_F(testHelloWorld, ArrayCmpEg6)
{

    uint8_t u8VirtualScreen[23];
    uint8_t u8ReferenceScreen[23] = {'H', 'a', 'l', 'l', 'o', ' ', 'W', 'o',
                                     'r', 'l', 'd', '!', 0,   0,   0,   0,
                                     0,   0,   0,   0,   0,   0};

    bool bResult;

    bResult = helloWorld_main(&u8VirtualScreen[0], sizeof(u8VirtualScreen));
    EXPECT_EQ(true, bResult);


    ASSERT_THAT(u8VirtualScreen, ContainerEq(u8ReferenceScreen));
}
*/
