/**
 * \file gpio.c
 * \brief GPIO driver functions required by the API
 */
#include "gpio.h"

#include "stm32f4xx_hal.h"

/**
 * \brief Initialise a port pin, call this before attempting to use it
 * \param in_port_instance instance of the port pin that is to be initialised,
 * this must be stored in persistent memory
 * \param in_port_addr - address for the port
 * \param in_pin  - the pin to initialise
 * \param in_state - what state should the pin be set to at initialisation
 **/
void drv_Gpio_Initialise(st_gpio_instance_t* in_port_instance,
                         const gpio_port_t* in_port_addr,
                         const gpio_pin_t in_pin,
                         const e_gpio_states_t in_state)
{

    if ( in_port_instance && in_port_addr ) {
        in_port_instance->pPort      = in_port_addr;
        in_port_instance->tPin       = in_pin;
        in_port_instance->eInitState = in_state;
        if ( in_state == eGpioLo ) {
            HAL_GPIO_WritePin((GPIO_TypeDef*)in_port_instance->pPort,
                              in_port_instance->tPin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin((GPIO_TypeDef*)in_port_instance->pPort,
                              in_port_instance->tPin, GPIO_PIN_SET);
        }
        in_port_instance->bInitialised = true;
    }
}
/**
 * \brief toggle a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 **/
void drv_Gpio_TogglePin(const st_gpio_instance_t* in_port_instance)
{
    if ( in_port_instance ) {
        if ( in_port_instance->bInitialised ) {
            HAL_GPIO_TogglePin((GPIO_TypeDef*)in_port_instance->pPort,
                               in_port_instance->tPin);
        }
    }
}

/**
 * \brief Set the output value to a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 * \param in_state - the enum value to write to the pin
 */
void drv_Gpio_WritePin(const st_gpio_instance_t* in_port_instance,
                       e_gpio_states_t in_state)
{
    if ( in_port_instance ) {
        if ( in_port_instance->bInitialised ) {
            if ( in_state == eGpioHi ) {
                HAL_GPIO_WritePin((GPIO_TypeDef*)in_port_instance->pPort,
                                  in_port_instance->tPin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin((GPIO_TypeDef*)in_port_instance->pPort,
                                  in_port_instance->tPin, GPIO_PIN_RESET);
            }
        }
    }
}

/**
 * \brief Read the state of a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 * \return the current state of the pin
 */
e_gpio_states_t drv_Gpio_ReadPin(const st_gpio_instance_t* in_port_instance)
{
    e_gpio_states_t eResult = eGpioUnknown;

    if ( in_port_instance ) {
        if ( in_port_instance->bInitialised ) {
            if ( HAL_GPIO_ReadPin((GPIO_TypeDef*)in_port_instance->pPort,
                                  in_port_instance->tPin) ) {
                eResult = eGpioHi;
            } else {
                eResult = eGpioLo;
            }
        }
    }

    return eResult;
}
