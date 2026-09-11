/**
 * \file cCounter.h
 * \brief Exposes counting module to demonstrate test fixtures
 */
#ifndef counter_H__
#define counter_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/**
 * \brief Initialise the counter function. Call this first.
 */
void counter_initialise(void);

/**
 * \brief check whether the counter setup
 * \return true if the counter has been initialised
 */
bool counter_isInitialised(void);

/**
 * \brief get the value of the counter
 * \return current counter value
 */
uint32_t counter_get(void);

/**
 * \brief increment the counter
 */
void counter_increment(void);

/**
 * \brief De-Initialise the counter function
 */
void counter_deInitialise(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* counter_H__ */
