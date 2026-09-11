/**
 * \file test_Main.cpp
 * \brief Tester exercising testing of private functions
 */
#include <gtest/gtest.h>

#include "helperMain.h"
#include "utilUnitTestWrapper.h"

struct test_Main : public ::testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
    void MyDelay()
    {
        struct timespec f_stGpTimer;
        const long f_cslDelayNs = 60000000L; /* 60ms */
        f_stGpTimer.tv_sec      = 0;
        f_stGpTimer.tv_nsec     = f_cslDelayNs;
        nanosleep(&f_stGpTimer, NULL);
    }
    void MySetUp()
    {
        MyDelay();
        ASSERT_EQ(true, helperMain_MainInit());
        MyDelay();
        MyDelay();
        ASSERT_EQ(true, helperMain_MainCreateThreads());
    }
    /* close once we have finished all the tests here */
    void MyTearDown()
    {
        helperMain_MainCleanUp();
        helperMain_MainWaitTermination();
    }
};

TEST_F(test_Main, NotInitialised)
{
    ASSERT_EQ(false, helperMain_IsInitialised());
}

TEST_F(test_Main, ThreadsNotCreated)
{
    EXPECTING_ASSERTS(2);

    ASSERT_EQ(false, helperMain_MainCreateThreads());

    EXPECT_TRUE(DID_ASSERT());
    EXPECTING_ASSERTS(0);
}

TEST_F(test_Main, DummySetUp)
{
    MySetUp();
    ASSERT_EQ(true, helperMain_IsInitialised());
}

TEST_F(test_Main, SigHandler)
{
    helperMain_SendSigInt();
    ASSERT_EQ(helperMain_IsKeepRunning(), false);
}

TEST_F(test_Main, Initialise)
{
    ASSERT_EQ(true, helperMain_IsInitialised());
}

TEST_F(test_Main, Restart)
{
    MySetUp();
    ASSERT_EQ(true, helperMain_IsInitialised());
    helpermain_shutdownRequest();
    helperMain_MainCleanUp();
    ASSERT_EQ(false, helperMain_IsInitialised());
    MySetUp();
}

TEST_F(test_Main, DeInit)
{
    MyTearDown();
    ASSERT_EQ(helperMain_IsInitialised(), false);
}

TEST_F(test_Main, MainLoopDebug)
{
    printf("debug\n");
    const char* argv[2] = {"test_Main", "-d"};
    helperMain_TearDown();
    ASSERT_EQ(0, test_main(2, (char**)(argv)));
}

TEST_F(test_Main, MainLoopCoverage)
{
    printf("coverage\n");
    const char* argv[2] = {"test_Main"
                           "-c"};
    helperMain_TearDown();
    ASSERT_EQ(0, test_main(2, (char**)(argv)));
}

TEST_F(test_Main, MainLoopNoParams)
{
    printf("no parameters\n");
    const char* argv[2] = {"test_Main"};
    helperMain_TearDown();
    ASSERT_EQ(0, test_main(2, (char**)(argv)));
}

