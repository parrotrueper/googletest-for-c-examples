/**
 * \file cEvaluate.c
 * \brief Collection of functions to use as unit under test for evaluation of
 * unit testing frameworks
 */
#include <math.h>
#include <stdio.h>

#include "evaluate.h"

static char f_result_str[5]; /* 0xXX\0*/
/**
 * \brief find the width of float for this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfFloat(void)
{
    return (sizeof(float) * 8);
}

/**
 * \brief find the width of double for this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfDouble(void)
{
    return (sizeof(double) * 8);
}

/**
 * \brief find the width of long for this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfLong(void)
{
    return (sizeof(long) * 8);
}

/**
 * \brief find the width of long doublefor this platform
 * \returns the number of bits
 */
uint32_t evaluate_platformSizeOfLongDouble(void)
{
    return (sizeof(long double) * 8);
}

/**
 * \brief get the 32-bit value of Pi
 */
float32_t evaluate_getPi32bit(void)
{
    float32_t float_pi_32 = ((float32_t)(M_PI));
    return float_pi_32 + (float32_t)(sin(float_pi_32));
}

/**
 * \brief get the 64-bit value of Pi
 */
float64_t evaluate_getPi64bit(void)
{
    float64_t float_pi_64 = 3.14159265358979323846;
    return float_pi_64 + sin(float_pi_64);
}

/**
 * \brief Divide two integers
 * \param in_numerator
 * \param in_denominator
 * \return in_numerator/in_denominator
 */
uint32_t evaluate_divide(uint32_t in_numerator, uint32_t in_denominator)
{
    return in_numerator / in_denominator;
}

/**
 * \brief Print contents of address
 * \param i_pAddress - pointer
 */
char* evaluate_showAddress(uint8_t* in_pAddress)
{
    sprintf((char*)(&f_result_str[0]), "0x%x", *in_pAddress);
    return &f_result_str[0];
}

/* EOF */
