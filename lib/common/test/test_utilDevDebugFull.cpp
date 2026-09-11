
#include <gtest/gtest.h>
#include <string.h>

#include "mString.h"
#include "utilDevDebug.h"

static bool f_bSafeCalled     = false;
static bool f_bConsumerCalled = false;
static char f_cPubMsg[512];
static char f_expected_message[512];
static int32_t f_line_num;
static const char* f_function;
static const char* f_expression;
static const char* f_filename;

#define ASSERT_STRING                                                          \
    "{\"BUG\":\"ASSERTION "                                                    \
    "FAILED\",\"in\":\"%s\",\"at\":\"%s\",\"Line\":%d,\"Assert\":\"%s\"}"

static uint16_t f_u16Len = 0;

using namespace testing;

struct testUtilDevDebug : public ::testing::Test {
    virtual void SetUp() override
    {
        f_bSafeCalled     = false;
        f_bConsumerCalled = false;
        f_cPubMsg[0]      = '\0';
        f_u16Len          = 0;
    }
    virtual void TearDown() override {}
};

void SafeFunction(void)
{
    f_bSafeCalled = true;
}
void ConsumerFunction(void)
{
    f_bConsumerCalled = true;
}
void PublishFunction(const char* in_filename, const char* in_function,
                     const char* in_expression, int32_t in_line_number)
{
    EXPECT_GT(snprintf(f_cPubMsg, sizeof(f_cPubMsg), ASSERT_STRING, in_filename,
                       in_function, in_line_number, in_expression),
              0);
    snprintf(f_expected_message, sizeof(f_expected_message), ASSERT_STRING,
             f_filename, f_function, f_line_num, f_expression);
    EXPECT_STREQ(f_expected_message, f_cPubMsg);
}

TEST_F(testUtilDevDebug, AssertsOnFalse)
{
    EXPECTING_ASSERTS(1);
    {
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebug, AssertHandlingNoCallbacks)
{
    utilDevDebug_initialise(NULL, NULL, NULL);
    EXPECTING_ASSERTS(1);
    {
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebug, AssertHandlingWithCallback)
{
    utilDevDebug_initialise(&ConsumerFunction, &PublishFunction, &SafeFunction);
    EXPECTING_ASSERTS(1);
    {
        f_function   = "TestBody()";
        f_expression = "(false)";
        f_filename   = __FNAME__;
        f_line_num   = __LINE__ + 1;
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
        EXPECT_TRUE(f_bSafeCalled);
        EXPECT_TRUE(f_bConsumerCalled);
        EXPECT_TRUE(f_cPubMsg[0] != '\0');
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebug, ErrorHandler)
{
    utilDevDebug_initialise(&ConsumerFunction, &PublishFunction, &SafeFunction);
    EXPECTING_ASSERTS(1);
    {
        f_function   = "TestBody()";
        f_filename   = __FNAME__;
        f_expression = "(ERROR_HANDLER)";
        f_line_num   = __LINE__ + 1;
        Error_Handler();
        EXPECT_TRUE(DID_ASSERT());
        EXPECT_TRUE(f_bSafeCalled);
        EXPECT_TRUE(f_bConsumerCalled);
        EXPECT_TRUE(f_cPubMsg[0] != '\0');
    }
    EXPECTING_ASSERTS(0);
}
