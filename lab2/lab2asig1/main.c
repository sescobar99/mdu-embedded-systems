//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"

/**
 * Configurations
 */
// Configure the UART.
void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Enable the pins of GPIOA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // Enable the UART 0
    GPIOPinConfigure(GPIO_PA0_U0RX); // Configure pinout A0
    GPIOPinConfigure(GPIO_PA1_U0TX); // Configure pinout A1
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Configure pins for use by UART peripheral
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); // Set the baud clock source for the specified UART
    UARTStdioConfig(0, 115200, 16000000); // Configure the UART console
}

/**
 * main.c
 */
int main(void)
{

    // Call to config. function defined before
    ConfigureUART();

    // Set the clock frequency
    float pwm_word;
    uint32_t systemClock;
    systemClock = SysCtlClockFreqSet(
            (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
                    | SYSCTL_CFG_VCO_480),
            16000);

    pwm_word = systemClock / 200;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Enable the pins of GPIOF
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); // Set the PWM clock config. (setting the rate in comparasion with the processor clock)
    // Reset the PWM
    SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    GPIOPinConfigure(GPIO_PF2_M0PWM2); // Configure pin F2
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2); // Configure pins for use by UART peripheral

    PWMGenConfigure(
            PWM0_BASE, PWM_GEN_1,
            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN); // Configure the PWM generator
    // PWM_GEN_MODE_DOWN: generates a left-aligned signal
    // PWM_GEN_MODE_NO_SYNC: set a asynch gen.
    // PWM_GEN_MODE_DBG_RUN: stills runs even is the processor is not
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwm_word); // Set the PWM generator clock period
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_word / 100); // Set the PWM generator pulse width
    PWMGenEnable(PWM0_BASE, PWM_GEN_1); // Enable the PWM generator
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true); // Enable the output of PWM0

    /*
     SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

     while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
     {
     }

     UARTConfigSetExpClk(
     UART0_BASE, SysCtlClockGet(), 38400,
     (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
     */

    unsigned char inputBuffer;

    while (1)
    {
        inputBuffer = UARTCharGet(UART0_BASE);
        UARTprintf(inputBuffer);
    }

    return 0;
}
