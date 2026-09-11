/**
 * \file helperMain.cpp
 * \brief Test helper demonstrating how to test private functions if that is
 * your sort of thing, but really, you only want to be testing the public API.
 */
#include "helperMain.h"

#include "main.c"

/** Wrapper TESTING Helper functions for the private functions */
bool helperMain_MainInit(void)
{
    return main_setup();
}

bool helperMain_MainCreateThreads(void)
{
    return main_createThreads();
}

void helperMain_MainCleanUp(void)
{
    DD_MAIN_TRACE("Entry %s\n", __FUNC__);
    main_teardown();
    DD_MAIN_TRACE("Exit %s\n", __FUNC__);
}

void helperMain_MainWaitTermination(void)
{
    DD_MAIN_TRACE("Entry %s\n", __FUNC__);
    main_waitTermination();
    DD_MAIN_TRACE("Exit %s\n", __FUNC__);
}

bool helperMain_IsInitialised(void)
{
    return f_main_initialised;
}


void helperMain_SendSigInt(void)
{
    main_signalHandler(0);
}

bool helperMain_IsKeepRunning(void)
{
    bool result = true;
    if ( f_keep_running == 0 ) {
        result = false;
    }
    return result;
}

void helpermain_shutdownRequest(void)
{
    main_shutdownRequest();
}

void helperMain_TearDown(void)
{
    f_debug_enabled      = false;
    f_main_initialised   = false;
    f_keep_running       = 1;
    f_shutdown_requested = false;
}
