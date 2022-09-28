// Core library for code-sense
#include "Energia.h"

// Include application, user and local libraries
#include <SPI.h>

#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
//Screen_HX8353E myScreen;

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

// Define variables and constants
#define joystickX 2
#define joystickY 26
uint16_t x, y, x00, y00;
uint32_t z;

float pwm_word;
uint32_t systemClock;
// Add setup code
void setup()
{
    x00 = 0;
    y00 = 0;
//    pinMode(ledGreen, OUTPUT);
//    pinMode(ledBlue, OUTPUT);
//    pinMode(ledRed, OUTPUT);

    // Set the clock frequency

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

    GPIOPinConfigure (GPIO_PF2_M0PWM2); // Configure pin F2
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2); // Configure pins for use by UART peripheral

    PWMGenConfigure(
            PWM0_BASE, PWM_GEN_1,
            PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwm_word);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_word / 100); // Set the PWM generator pulse width
    PWMGenEnable(PWM0_BASE, PWM_GEN_1); // Enable the PWM generator
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true); // Enable the output of PWM0
}

// Add loop code
void loop()
{
//    analogWrite(ledRed, 254);
    x = map(analogRead(joystickX), 0, 4096, 0, 128);
    y = map(analogRead(joystickY), 0, 4096, 128, 0);
    if (x < 1)
        x = 1;
    if (x > 126)
        x = 126;
    if (y < 1)
        y = 1;
    if (y > 126)
        y = 126;

//    digitalWrite(ledBlue, HIGH);
//    digitalWrite(ledGreen, HIGH);
    if ((x00 != x) || (y00 != y))
    {
        z = (uint32_t) ((x - 64) * (x - 64) + (y - 64) * (y - 64)) >> 8;

//        digitalWrite(ledGreen, HIGH);
    }

    if (z <= 0)
    {
        z = 0.0f;
    }
    else if (z >= 90)
    {
        z = 100.0f;
    }else{
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (pwm_word / 10) * z); // Set the PWM generator pulse width
    }



//        if (z > 4)      colour = redColour;
//        else if (z > 1) colour = yellowColour;
//        else            colour = greenColour;
//
//        myScreen.dRectangle(x00-1, y00-1, 3, 3, blackColour);
//        myScreen.dRectangle(x-1, y-1, 3, 3, colour);
//        x00 = x;
//        y00 = y;

//    myScreen.gText(0, myScreen.screenSizeY()-myScreen.fontSizeY(),
//                   "x=" + i32toa((int16_t)x-64, 10, 1, 6) +" y=" + i32toa(64-(int16_t)y, 10, 1, 6),
//                   colour);
}
