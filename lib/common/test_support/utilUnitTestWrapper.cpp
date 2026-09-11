/**
 * \file utilUnitTestWrapper.cpp
 * \brief C wrappers for C++ unit testing framework
 */
#include "utilUnitTestWrapper.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define ALLOWING_VERBOSE_EN (1)

static uint32_t f_u32PermCount = 0;
static uint32_t f_u32PrevCount = 0;

void util_UTWrapper_Assert(const char* i_filename, const char* i_module,
                           uint32_t i_u32Line, const char* i_Condition)
{
    if ( f_u32PermCount == 0 ) {
        (void)printf("TEST: ASSERTION FAILED in: %s %s(), L:%d, A:(%s)\n",
                     i_filename, i_module, i_u32Line, i_Condition);
        /* for gtest we need to send to stderr */
        std::cerr << "TEST: ASSERTION FAILED in:" << i_filename << i_module
                  << "(), L:" << i_u32Line << "A:(" << i_Condition << ")"
                  << std::endl;
        exit(222);
    } else {
#if ALLOWING_VERBOSE_EN

        (void)printf("ALLOWING[%d,%d]: ASSERTION in %s\n", f_u32PermCount,
                     f_u32PrevCount, i_module);
        /*std::cerr << "ALLOWING[" << f_u32PermCount << "," << f_u32PrevCount
                  << "]: ASSERTION in " << i_module << std::endl;*/
#endif /* ALLOWING_VERBOSE_EN */

        f_u32PermCount--;
    }
}

/**
 * \brief Unit test helper to prevent an assert halting a test. Called
 * when the test is designed to deliberately blow an assert
 * \param i_u32NumOfAsserts - number of asserts to forgive (suggested max 3)
 **/
void util_UTWrapper_AllowAsserts(uint32_t i_u32NumOfAsserts)
{
    f_u32PermCount = i_u32NumOfAsserts;
    f_u32PrevCount = i_u32NumOfAsserts;
}

/**
 * \brief Unit test helper to check if an assert triggered. Called after
 * asking to permit the assert.
 */
bool util_UTWrapper_DidAsserted(void)
{
    bool bResult = false;

    if ( f_u32PrevCount != f_u32PermCount ) {
        f_u32PrevCount = f_u32PermCount;
        bResult        = true;
    }
    return bResult;
}
