/**
 * \file helloWorld.c
 * \brief Example file
 */
#include "helloWorld.h"

#include "utilDevDebug.h"

/**
 * \brief Write Hello world to a buffer of specified length
 * \param in_dest - pointer to the destination buffer
 * \param in_length - length of the message in bytes
 * \return true if the entire message was transferred to its destination
 */
bool helloWorld_main(uint8_t* in_dest, uint32_t in_length)
{
    /* #lizard forgives this example with complexity 8 */
    bool result             = true;
    const uint8_t message[] = {"Hello World!"};
    uint32_t ix;
    uint32_t limit = (uint32_t)(sizeof(message));

    DD_ASSERT(NULL != in_dest);
    DD_ASSERT((uint32_t)(0) != in_length);

    /* sanity check the input parameters */
    if ( (in_dest == NULL) || (in_length < (uint32_t)(1)) ) {
        result = false;
    } else if ( (in_dest != NULL) && (in_length == (uint32_t)(1)) ) {
        in_dest[0] = 0;
        result     = false;
    } else {
        /* ensure we do not over run the target buffer */
        if ( limit > in_length ) {
            limit  = in_length;
            result = false;
        }
        /* copy the message to the destination */
        for ( ix = 0; ix < (limit - (uint32_t)(1)); ix++ ) {
            in_dest[ix] = message[ix];
        }
        /* null terminate the rest of the buffer */
        for ( ; ix < in_length; ix++ ) {
            in_dest[ix] = 0;
        }
    }

    return result;
}
