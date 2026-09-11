/**
 * \file helperMain.h
 * \brief Test helper demonstrating how to test private functions
 */
#ifndef helperMain_H__
#define helperMain_H__

#include <stdbool.h>
#include <stdint.h>

#include "globaltypes.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**************************************
 *     PRIVATE MAIN WRAPPER HELPERS
 ***************************************/
bool helperMain_IsInitialised(void);
void helperMain_SendSigInt(void);
bool helperMain_IsKeepRunning(void);

bool helperMain_MainInit(void);
bool helperMain_MainCreateThreads(void);
void helperMain_MainCleanUp(void);
void helperMain_MainWaitTermination(void);
void helpermain_shutdownRequest(void);
int_t test_main(int_t argc, char** argv);
void helperMain_TearDown(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* helperMain_H__ */

