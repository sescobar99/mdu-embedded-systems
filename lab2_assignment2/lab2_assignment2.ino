/**
 * MDU ES Lab2 Assignment2
 * Add another function, which uses the potentiometer values as the basis of input instead of the UART.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */
//*****************************************************************************
#include "Energia.h"
#include <SPI.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"

#define joystickX 2
#define joystickY 26
uint16_t x, y;
float joystickCompoundValue;
float inputNumber;
float pwm_word;
uint32_t systemClock;


void setup()
{
    inputNumber = 0.0f;
    ConfigureUART();
    ConfigurePWM(&pwm_word);
}


void loop()
{
    //Read values from the joystick and maps them between 0-128
    x = map(analogRead(joystickX), 0, 4096, 0, 128);
    y = map(analogRead(joystickY), 0, 4096, 128, 0);

    //Gets the joystick distance from the center a assigns it to "inputNumber"
    joystickCompoundValue = ((x - 64) * (x - 64) + (y - 64) * (y - 64)) >> 5;
    inputNumber = joystickCompoundValue;

    //Handles edge cases if inputNumber is NaN or <=0 maps it to 0. If greater than 100 maps to 100
    if (inputNumber <= 0 || inputNumber >= 100)
    {
        inputNumber = (inputNumber <= 0 ? 0.0f : 100.0f);

        //Disable PWM and enable digital output
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
                     (inputNumber <= 0 ? 0 : GPIO_PIN_3));

    }
    else // Handles values between (0,100)
    {
        //Enable PWM mode again
        GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinConfigure (GPIO_PF3_M0PWM3);
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

        //Changes LED intensity according user input
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,
                         (pwm_word / 100) * (inputNumber / 3)); //Pulse width is defined as the number of PWM clock ticks
    }

    UARTprintf("=> %d \n", (int) inputNumber);

}

// Configures the UART. (Enables peripherals, configure GPIO & clock)
void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Configure pins for use by UART peripheral
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); // Set the baud clock source for the specified UART
    UARTStdioConfig(0, 115200, 16000000); // Configure the UART console
}

//Configures the PWM. (enables peripherals, configures clock & pwm generator)
void ConfigurePWM(float *pwm_word)
{
    uint32_t systemClock;

    // This function configures the main system clocking for the device. ...FreqSet(required configuration, requested processor frequency)
    systemClock = SysCtlClockFreqSet(
            (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
                    | SYSCTL_CFG_VCO_480),
            16000);
//    UARTprintf("%d\n", systemClock); //Aprox 10000000
    *pwm_word = systemClock / 200;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Enable GPIO F pins

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); //Configures the rate of the clock provided to the PWM module as a ratio of the processor clock

    SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    //Configures pin(s) for use by the PWM peripheral.  ...PWM(address of the GPIO port, bit-packed representation of the pin/s)
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinConfigure (GPIO_PF3_M0PWM3);

    PWMGenConfigure(
            PWM0_BASE, PWM_GEN_1,
            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, *pwm_word); // Period of the generator block is defined as the number of PWM clock ticks between pulses on the generator

    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);

}

