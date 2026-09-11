
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

static uint16_t f_u16Len = 0;

using namespace testing;

struct testUtilDevDebugTiny : public ::testing::Test {
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
    (void)in_function;
    (void)in_expression;
    (void)in_line_number;
    /* for tiny assert publish function only the first parameter
    string is relevant and the line number is abused as a length of
    string, what should come out is:
    BUG:<filename capped to DD_ASSERT_MAX_FILENAME_LEN chars>,
    <function name capped to DD_ASSERT_MAX_FUNCTION_LEN chars>(),<line number>\n\0 */
    /* create the expected message */
    f_expected_message[0] = 'B';
    f_expected_message[1] = 'U';
    f_expected_message[2] = 'G';
    f_expected_message[3] = ':';
    uint16_t ix           = 4U;
    for ( uint16_t i = 0; i < DD_ASSERT_MAX_FILENAME_LEN; i++ ) {
        char c = f_filename[i];
        if ( c == '\0' ) {
            break;
        }
        f_expected_message[ix] = c;
        ix++;
    }
    f_expected_message[ix] = ',';
    ix++;
    for ( uint16_t i = 0; i < DD_ASSERT_MAX_FUNCTION_LEN; i++ ) {
        char c = f_function[i];
        if ( c == '\0' ) {
            break;
        }
        f_expected_message[ix] = c;
        ix++;
    }
    f_expected_message[ix] = '(';
    ix++;
    f_expected_message[ix] = ')';
    ix++;
    f_expected_message[ix] = ',';
    ix++;
    /* convert the line number to ASCII */
    char line[DD_ASSERT_MAX_LINENUM_LEN + 1U];
    line[DD_ASSERT_MAX_LINENUM_LEN] = '\0';
    (void)m_itoa_s(f_line_num, line, DD_ASSERT_MAX_LINENUM_LEN);
    for ( uint16_t i = 0; i < DD_ASSERT_MAX_LINENUM_LEN; i++ ) {
        char c = line[i];
        if ( c == '\0' ) {
            break;
        }
        f_expected_message[ix] = c;
        ix++;
    }
    f_expected_message[ix] = '\n';
    ix++;
    f_expected_message[ix] = '\0';

    f_cPubMsg[0] = '\0';
    /* copy over the message to the published buffer */
    for ( uint16_t i = 0; i < in_line_number; i++ ) {
        char c = in_filename[i];
        if ( c == '\0' ) {
            break;
        }
        f_cPubMsg[i] = c;
    }
    f_cPubMsg[in_line_number] = '\0';
    EXPECT_STREQ(f_expected_message, f_cPubMsg);
}

TEST_F(testUtilDevDebugTiny, AssertsOnFalse)
{
    EXPECTING_ASSERTS(1);
    {
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebugTiny, AssertHandlingNoCallbacks)
{
    utilDevDebug_initialise(NULL, NULL, NULL);
    EXPECTING_ASSERTS(1);
    {
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebugTiny, AssertHandlingWithCallback)
{
    utilDevDebug_initialise(&ConsumerFunction, &PublishFunction, &SafeFunction);
    EXPECTING_ASSERTS(1);
    {
        f_function   = "TestBody";
        f_expression = "(false)";
        f_filename   = "test_utilDev"; /* filename is truncated to 12 chars */
        f_line_num   = __LINE__ + 1;
        DD_ASSERT(false);
        EXPECT_TRUE(DID_ASSERT());
        EXPECT_TRUE(f_bSafeCalled);
        EXPECT_TRUE(f_bConsumerCalled);
        EXPECT_TRUE(f_cPubMsg[0] != '\0');
    }
    EXPECTING_ASSERTS(0);
}

TEST_F(testUtilDevDebugTiny, ErrorHandler)
{
    utilDevDebug_initialise(&ConsumerFunction, &PublishFunction, &SafeFunction);
    EXPECTING_ASSERTS(1);
    {
        f_function   = "TestBody";
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
