/**
 * \file HelloWorld.h
 * \brief Example file
 */
#ifndef helloWorld_H__
#define helloWorld_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**
 * \brief Write Hello world to a buffer of specified length
 * \param in_dest - pointer to the destination buffer
 * \param in_length - length of the message in bytes
 * \return true if the entire message was transfered to its destination
 */
bool helloWorld_main(uint8_t* in_dest, uint32_t in_length);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* HELLO_WORLD_H__ */
