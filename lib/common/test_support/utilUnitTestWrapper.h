/**
 * \file utilUnitTestWrapper.h
 * \brief exposes C wrappers for C++ unit testing framework functions
 */
#ifndef UTIL_UTWRAPPER_H__
#define UTIL_UTWRAPPER_H__

#include <stdbool.h>
#include <stdint.h>

#ifndef DD_UNIT_TESTING
    #error "DD_UNIT_TESTING must be defined to include utilUnitTestWrapper.h"
#endif /* DD_UNIT_TESTING */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

void util_UTWrapper_Assert(const char* i_filename, const char* i_module,
                           uint32_t i_u32Line, const char* i_Condition);

/**
 * \brief Unit test helper to prevent an assert halting a test. Called
 * when the test is designed to deliberately blow an assert
 * \param i_u32NumOfAsserts - number of asserts to permit (suggested max 3)
 **/
void util_UTWrapper_AllowAsserts(uint32_t i_u32NumOfAsserts);

/**
 * \brief Unit test helper to check if an assert triggered. Called after
 * asking to permit the assert.
 */
bool util_UTWrapper_DidAsserted(void);

#define DID_ASSERT() util_UTWrapper_DidAsserted()

#define EXPECTING_ASSERTS(number_of_times)                                     \
    util_UTWrapper_AllowAsserts(number_of_times)

#define EXPECT_TO_ASSERT(what_should_assert)                                   \
    do {                                                                       \
        what_should_assert;                                                    \
        EXPECT_TRUE(util_UTWrapper_DidAsserted());                             \
    } while ( 0 )


#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* UTIL_UTWRAPPER_H__ */
