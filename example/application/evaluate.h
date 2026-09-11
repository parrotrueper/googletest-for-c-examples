/**
 * \file cEvaluate.h
 * \brief Exposes function to be used for unit testing
 */
#ifndef evaluate_H__
#define evaluate_H__

#include <stdbool.h>
#include <stdint.h>

#include "globaltypes.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**
 * \brief Find the width in bits for a float on this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfFloat(void);

/**
 * \brief find the width of double for this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfDouble(void);

/**
 * \brief find the width of long double for this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfLongDouble(void);

/**
 * \brief get the 32-bit value of Pi
 */
float32_t evaluate_getPi32bit(void);

/**
 * \brief get the 64-bit value of Pi
 */
float64_t evaluate_getPi64bit(void);

/**
 * \brief Divide two integers
 * \param in_numerator
 * \param in_denominator
 * \return in_numerator/in_denominator
 */
uint32_t evaluate_divide(uint32_t in_numerator, uint32_t in_denominator);

/**
 * \brief Print contents of address
 * \param in_pAddress - pointer
 */
char* evaluate_showAddress(uint8_t* in_pAddress);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* evaluate_H__ */
