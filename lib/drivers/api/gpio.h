/**
 * \file gpio.h
 * \brief Exposes the GPIO driver functions required by the API
 */
#ifndef DRV_GPIO_H__
#define DRV_GPIO_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

typedef uint32_t gpio_port_t; /* platform dependent types */
typedef uint16_t gpio_pin_t;  /* platform dependent types */

typedef enum {
    eGpioLo = 0,
    eGpioHi,
    eGpioUnknown
    /* add rd pin options here like, hi-z, push-pull, open-collector etc... */
} e_gpio_states_t;

typedef struct {
    bool bInitialised;
    const gpio_port_t* pPort;
    gpio_pin_t tPin;
    e_gpio_states_t eInitState;
} st_gpio_instance_t;

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
                         const e_gpio_states_t in_state);
/**
 * \brief toggle a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 **/
void drv_Gpio_TogglePin(const st_gpio_instance_t* in_port_instance);

/**
 * \brief Set the output value to a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 * \param in_state - the enum value to write to the pin
 */
void drv_Gpio_WritePin(const st_gpio_instance_t* in_port_instance,
                       e_gpio_states_t in_state);

/**
 * \brief Read the state of a port pin
 * \param in_port_instance - the instance for the port pin, this must have
 * been previously initialised
 * \return the current state of the pin
 */
e_gpio_states_t drv_Gpio_ReadPin(const st_gpio_instance_t* in_port_instance);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* DRV_GPIO_H__ */
