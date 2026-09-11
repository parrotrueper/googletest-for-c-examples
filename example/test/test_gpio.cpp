/**
 * \file test_gpio.cpp
 * \brief Tester exercising mocking a HAL
 */
#include <gtest/gtest.h>

#include "gmock/gmock.h"
#include "gpio.h"          /* UUT */
#include "stm32f4xx_hal.h" /* prototype for the mock function */

using namespace ::testing;

#define POWER_LED_PIN  GPIO_PIN_0
#define POWER_LED_PORT GPIOB

/* Mock HAL GPIO
==============================================================================*/

/**
 *  \brief GMock that defines the API
 */
class MockHalGpio
{
  public:
    virtual ~MockHalGpio() {}

    /* mock API */
    MOCK_METHOD(void, HAL_GPIO_TogglePin,
                (GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin));
    MOCK_METHOD(void, HAL_GPIO_WritePin,
                (GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin,
                 GPIO_PinState PinState));
    MOCK_METHOD(GPIO_PinState, HAL_GPIO_ReadPin,
                (GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin));
};

/**
 * \brief GTest Fixture to instantiate the test class
 * \param _halGpioMock pointer to the test class that enables external access to the test object
 */
class GpioUnitTest : public ::testing::Test
{
  public:
    GpioUnitTest()
    {
        /* allocate the mock object */
        _halGpioMock.reset(new ::testing::NiceMock<MockHalGpio>());
    }

    ~GpioUnitTest()
    {
        /* free the mock object */
        _halGpioMock.reset();
    }
    virtual void SetUp() {}
    virtual void TearDown() {}

    // pointer for accessing mocked library
    static std::unique_ptr<MockHalGpio> _halGpioMock;
};

/* make the class static member accessible to other function */
std::unique_ptr<MockHalGpio> GpioUnitTest::_halGpioMock;


/* mock functions
==============================================================================*/
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GpioUnitTest::_halGpioMock->HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,
                       GPIO_PinState PinState)
{
    GpioUnitTest::_halGpioMock->HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return GpioUnitTest::_halGpioMock->HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

/*  The Unit Tests
==============================================================================*/
TEST_F(GpioUnitTest, InitialiseHi)
{

    st_gpio_instance_t stPowerLed = {.bInitialised = false,
                                     .pPort        = nullptr,
                                     .tPin         = 0,
                                     .eInitState   = eGpioLo};
    gpio_port_t* pLedPort         = (gpio_port_t*)POWER_LED_PORT;
    gpio_pin_t tLedPin            = POWER_LED_PIN;

    EXPECT_CALL(*_halGpioMock, HAL_GPIO_WritePin((GPIO_TypeDef*)pLedPort,
                                                 tLedPin, GPIO_PIN_SET))
        .Times(1);
    drv_Gpio_Initialise(&stPowerLed, pLedPort, tLedPin, eGpioHi);

    EXPECT_EQ(true, stPowerLed.bInitialised);
    EXPECT_EQ(pLedPort, stPowerLed.pPort);
    EXPECT_EQ(tLedPin, stPowerLed.tPin);
}

TEST_F(GpioUnitTest, InitialiseLo)
{

    st_gpio_instance_t stPowerLed = {.bInitialised = false,
                                     .pPort        = nullptr,
                                     .tPin         = 0,
                                     .eInitState   = eGpioLo};
    gpio_port_t* pLedPort         = (gpio_port_t*)POWER_LED_PORT;
    gpio_pin_t tLedPin            = POWER_LED_PIN;

    EXPECT_CALL(*_halGpioMock, HAL_GPIO_WritePin((GPIO_TypeDef*)pLedPort,
                                                 tLedPin, GPIO_PIN_RESET))
        .Times(1);
    drv_Gpio_Initialise(&stPowerLed, pLedPort, tLedPin, eGpioLo);

    EXPECT_EQ(true, stPowerLed.bInitialised);
    EXPECT_EQ(pLedPort, stPowerLed.pPort);
    EXPECT_EQ(tLedPin, stPowerLed.tPin);
}

TEST_F(GpioUnitTest, WritePin)
{

    gpio_port_t* expectPort          = (gpio_port_t*)POWER_LED_PORT;
    gpio_pin_t expectedPin           = POWER_LED_PIN;
    e_gpio_states_t expectStateSet   = eGpioHi;
    e_gpio_states_t expectStateReset = eGpioLo;
    st_gpio_instance_t Led           = {.bInitialised = true,
                                        .pPort        = expectPort,
                                        .tPin         = expectedPin,
                                        .eInitState   = eGpioLo};

    EXPECT_CALL(*_halGpioMock,
                HAL_GPIO_WritePin((GPIO_TypeDef*)expectPort, expectedPin,
                                  (GPIO_PinState)expectStateSet))
        .Times(1);
    EXPECT_CALL(*_halGpioMock,
                HAL_GPIO_WritePin((GPIO_TypeDef*)expectPort, expectedPin,
                                  (GPIO_PinState)expectStateReset))
        .Times(1);

    drv_Gpio_WritePin(&Led, eGpioHi);
    drv_Gpio_WritePin(&Led, eGpioLo);
}

TEST_F(GpioUnitTest, ReadPin)
{

    gpio_port_t* expectedPort        = (gpio_port_t*)POWER_LED_PORT;
    gpio_pin_t expectedPin           = POWER_LED_PIN;
    e_gpio_states_t expectStateSet   = eGpioHi;
    e_gpio_states_t expectStateReset = eGpioLo;
    st_gpio_instance_t Led           = {.bInitialised = true,
                                        .pPort        = expectedPort,
                                        .tPin         = expectedPin,
                                        .eInitState   = eGpioLo};

    EXPECT_CALL(*_halGpioMock,
                HAL_GPIO_ReadPin((GPIO_TypeDef*)expectedPort, expectedPin))
        .Times(2)
        .WillOnce(Return((GPIO_PinState)expectStateSet))
        .WillOnce(Return((GPIO_PinState)expectStateReset));

    e_gpio_states_t actualStateSet = drv_Gpio_ReadPin(&Led);
    EXPECT_EQ(expectStateSet, actualStateSet);

    e_gpio_states_t actualStateReset = drv_Gpio_ReadPin(&Led);
    EXPECT_EQ(expectStateReset, actualStateReset);
}

TEST_F(GpioUnitTest, TogglePin)
{
    gpio_port_t* expectedPort = (gpio_port_t*)POWER_LED_PORT;
    gpio_pin_t expectedPin    = POWER_LED_PIN;
    st_gpio_instance_t Led    = {.bInitialised = true,
                                 .pPort        = expectedPort,
                                 .tPin         = expectedPin,
                                 .eInitState   = eGpioLo};

    EXPECT_CALL(*_halGpioMock,
                HAL_GPIO_TogglePin((GPIO_TypeDef*)expectedPort, expectedPin))
        .Times(1);

    drv_Gpio_TogglePin(&Led);
}
