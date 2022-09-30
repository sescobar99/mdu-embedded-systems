/**
 * MDU ES Lab2 Assignment1
 * Write a program which lets the user decide on the brightness of a LED using the PWM library  for  the  embedded  system.
 * The  expressed  values  should  be  on  a  percentage  basis (between 0% and 100%), and should represent the LED intensity.
 * The program should be able to set any percentage of light.
 * The program must handle 3 different cases. Two special cases of 0% and 100% should turn off and on the LED using UART (functions for LEDs used in lab 1), respectively.
 * Other values more than 0% and less than 100% should be expressed using PWM library Button input.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */
//*****************************************************************************
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
#include "drivers/pinout.h"

//***********************************************************************
//                       Configurations
//***********************************************************************
// Configures the UART. (Enables peripherals, configure GPIO & clock)
void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
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
    GPIOPinConfigure(GPIO_PF3_M0PWM3);

    PWMGenConfigure(
            PWM0_BASE, PWM_GEN_1,
            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, *pwm_word); // Period of the generator block is defined as the number of PWM clock ticks between pulses on the generator

    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);

}

//***********************************************************************
// main.c
//***********************************************************************
int main(void)
{

    uint32_t ui32Len = 6;
    char pcBuf;
    float inputNumber = 0.0f;
    char menuMessage[] =
            "Insert light percentage values between 0 and 100 (NaN -> 0.0, input>100 -> 100.0)\n";
    float pwm_word;
    float lowerValue = 0.01f; //Initially 0 .But we had some troubles with values smaller than 0.01 (3 decimal digits)

    ConfigureUART();
    ConfigurePWM(&pwm_word);

    while (1)
    {
        //Prints message, waits for user input and parse it
        UARTprintf(menuMessage);
        UARTgets(&pcBuf, ui32Len);
        inputNumber = strtof(&pcBuf, NULL);

        //Handles edge cases if inputNumber is NaN or <"lowerValue" maps it to 0. If greater than 100 maps to 100
        if (inputNumber < lowerValue|| inputNumber >= 100.0f)
        {
            inputNumber = (inputNumber < lowerValue ? 0.0f : 100.0f);

            //Disable PWM and enable digital output
            PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
            GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
                         (inputNumber <= 0 ? 0 : GPIO_PIN_3));

        }
        else // Handles values between (lowerValue,100)
        {
            //Enable PWM mode again
            GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
            GPIOPinConfigure(GPIO_PF3_M0PWM3);
            PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

            //Changes LED intensity according user input
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,
                             (pwm_word / 100) * (inputNumber / 3)); //Pulse width is defined as the number of PWM clock ticks
        }

        UARTprintf("=> %d \n", (int) inputNumber);

    }
}

//*****************************************************************************
//                 Reference
//*****************************************************************************
//
//********uartstdio functions
//        UARTStdioConfig(ui32PortNum, ui32Baud, ui32SrcClock);
//        UARTwrite(*pcBuf,ui32Len);
//        UARTgets(*pcBuf, ui32Len);
//        UARTgetc();
//        UARTvprintf(*pcString, vaArgP);
//        UARTprintf(*pcString, *pcString2);
//********uart usage
//        while (!UARTCharsAvail(UART0_BASE))       {       }
//        UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
//         (UART0_BASE)

