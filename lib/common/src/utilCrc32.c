
/**
 * \file util_Crc32.c
 * \brief CRC-32 calculator
 */
#include <stdbool.h>
#include <stddef.h> /* NULL */

#include "utilCrc32.h" /* Our API */

/****************************************************************/
/* Private Macros                                               */
/****************************************************************/
#define UTIL_CRC32_LUT_SIZE   (256U)
#define UTIL_CRC32_POLYNOMIAL (0xEDB88320U)
#define UTIL_CRC32_SEED       (0xFFFFFFFFU)
/****************************************************************/
/* Private Variables */
/****************************************************************/
static bool f_bInitialised
    = false; /** set to true once the look up table has been populated */
static uint32_t f_u32CrcLUT[UTIL_CRC32_LUT_SIZE]; /** pre-calculated bit pattern
                                                     look up table */

/**
 * \brief Populate the look up table. Called the first time the calc function is
 * called. The table is generated once.
 **/
static void util_Crc32_Initialise(void)
{
    uint32_t u32Ix;
    uint32_t u32Bit;
    uint32_t u32Entry;

    for ( u32Ix = 0; u32Ix < UTIL_CRC32_LUT_SIZE; u32Ix++ ) {
        u32Entry = u32Ix;
        for ( u32Bit = 0; u32Bit < 8; u32Bit++ ) {
            if ( 0 != (u32Entry & (uint32_t)(0x1)) ) {
                u32Entry = (u32Entry >> 1) ^ (uint32_t)(UTIL_CRC32_POLYNOMIAL);
            } else {
                u32Entry = u32Entry >> 1;
            }
        }
        f_u32CrcLUT[u32Ix] = u32Entry;
    }
    f_bInitialised = true;
}

/**
 * \brief Calculate CRC-32 using polynomial 0xEDB88320, seed 0xFFFFFFFF.
 * \param i_pData pointer to buffer data
 * \param in_length size of the buffer in bytes
 */
uint32_t util_Crc32_Calc(const uint8_t* i_pData, uint32_t in_length)
{
    uint32_t u32Result = 0;
    uint32_t u32Ix;
    uint32_t u32LutIx;
    uint32_t u32XorVal;

    if ( i_pData != NULL ) {
        /* check that we have populated our LUT */
        if ( !f_bInitialised ) {
            util_Crc32_Initialise();
        }

        u32Result = (uint32_t)(UTIL_CRC32_SEED);

        for ( u32Ix = 0; u32Ix < in_length; u32Ix++ ) {

            u32XorVal = (uint32_t)(0xFF) & (uint32_t)(i_pData[u32Ix]);
            u32LutIx  = u32Result ^ u32XorVal;
            u32Result = (u32Result >> 8) ^ f_u32CrcLUT[(u32LutIx & 0xffU)];
        }

        u32Result ^= 0xffffffffU;
        u32Result = u32Result & 0xffffffffU;
    }
    return u32Result;
}
