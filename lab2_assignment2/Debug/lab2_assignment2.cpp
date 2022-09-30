#include "Energia.h"

#line 1 "C:/Users/josep/gitrepos/mdu-embedded-systems/lab2_assignment2/lab2_assignment2.ino"









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
void setup();
void loop();
void ConfigureUART(void);
void ConfigurePWM(float *pwm_word);

#line 28
uint16_t x, y, x00, y00;
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
    x = map(analogRead(joystickX), 0, 4096, 0, 128);
    y = map(analogRead(joystickY), 0, 4096, 128, 0);

    x = x < 1 ? 1 : x;
    x = x > 126 ? 126 : x;
    y = y < 1 ? 1 : y;
    y = y > 126 ? 126 : y;



        joystickCompoundValue = ((x - 64) * (x - 64) + (y - 64) * (y - 64))
                >> 5;
        inputNumber = joystickCompoundValue;



    if (inputNumber <= 0 || inputNumber >= 100)
    {
        inputNumber = (inputNumber <= 0 ? 0.0f : 100.0f);

        
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
                     (inputNumber <= 0 ? 0 : GPIO_PIN_3));

    }
    else
    {
        
        GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
        GPIOPinConfigure (GPIO_PF3_M0PWM3);
        PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,
                         (pwm_word / 100) * (inputNumber / 3)); 
    }

    UARTprintf("=> %d \n", (int) inputNumber);

}

void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); 
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); 
    UARTStdioConfig(0, 115200, 16000000); 
}

void ConfigurePWM(float *pwm_word)
{
    uint32_t systemClock;

    
    systemClock = SysCtlClockFreqSet(
            (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
                    | SYSCTL_CFG_VCO_480),
            16000);

    *pwm_word = systemClock / 200;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); 

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 

    SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinConfigure (GPIO_PF3_M0PWM3);

    PWMGenConfigure(
            PWM0_BASE, PWM_GEN_1,
            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, *pwm_word); 

    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);

}




