
/**
 * \file calcCrc32.c
 * \brief CRC-32 calculator
 */
#include <stdbool.h>
#include <stddef.h> /* NULL */

#include "calcCrc32.h" /* Our API */

#define CALC_CRC32_LUT_LEN    (256U)
#define CALC_CRC32_POLYNOMIAL (0xEDB88320U)
#define CALC_CRC32_SEED       (0xFFFFFFFFU)

static bool f_crc32_initialised
    = false; /** set to true once the look up table has been populated */
static uint32_t f_crc32_lut[CALC_CRC32_LUT_LEN]; /** pre-calculated bit pattern
                                                     look up table */
/*******************************************************************************
 *                         Private Functions
 ******************************************************************************/
/**
 * \brief Populate the look up table. Called the first time the calc function is
 * called. The table is generated once.
 **/
static void calcCrc32_Initialise(void)
{
    for ( uint32_t ix = 0; ix < CALC_CRC32_LUT_LEN; ix++ ) {
        uint32_t entry = ix;
        for ( uint32_t bits = 0; bits < 8U; bits++ ) {
            if ( 0U != (entry & (uint32_t)(0x1)) ) {
                entry = (entry >> 1) ^ (uint32_t)(CALC_CRC32_POLYNOMIAL);
            } else {
                entry = entry >> 1;
            }
        }
        f_crc32_lut[ix] = entry;
    }
    f_crc32_initialised = true;
}

/*******************************************************************************
 *                      Exposed Interfaces
 *****************************************************************************/
/**
 * \brief Calculate CRC-32 using polynomial 0xEDB88320, seed 0xFFFFFFFF.
 * \param in_data pointer to buffer data
 * \param in_length size of the buffer in bytes
 */
uint32_t calcCrc32(const uint8_t* in_data, uint32_t in_length)
{
    uint32_t result = 0;
    uint32_t lut_ix;
    uint32_t xor_value;

    if ( in_data != NULL ) {
        /* check that we have populated our LUT */
        if ( !f_crc32_initialised ) {
            calcCrc32_Initialise();
        }

        result = (uint32_t)(CALC_CRC32_SEED);

        for ( uint32_t ix = 0; ix < in_length; ix++ ) {

            xor_value = (uint32_t)(0xFF) & (uint32_t)(in_data[ix]);
            lut_ix    = result ^ xor_value;
            result = (result >> 8) ^ f_crc32_lut[(lut_ix & (uint32_t)(0xff))];
        }

        result ^= 0xffffffffU;
        result = result & 0xffffffffU;
    }
    return result;
}
