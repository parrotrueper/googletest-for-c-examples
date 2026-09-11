/**
 * \file counter.c
 * \brief A pointless counter module to demonstrate test fixtures
 */
#include "counter.h"

static bool f_counter_initialised = false;
static uint32_t f_counter         = 0;

/**
 * \brief Initialise the counter function. Call this first.
 */
void counter_initialise(void)
{
    f_counter_initialised = true;
    f_counter             = 0;
}

/**
 * \brief check whether the counter setup
 * \return true if the counter has been initialised
 */
bool counter_isInitialised(void)
{
    return f_counter_initialised;
}

/**
 * \brief get the value of the counter
 * \return current counter value
 */
uint32_t counter_get(void)
{
    return f_counter;
}

/**
 * \brief increment the counter
 */
void counter_increment(void)
{
    if ( f_counter_initialised ) {
        f_counter++;
    }
}

/**
 * \brief De-Initialise the counter function
 */
void counter_deInitialise(void)
{
    f_counter_initialised = false;
}

/* EOF */
