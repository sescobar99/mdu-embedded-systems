
/**
 * MDU ES Lab1 Assignment2
 * Write a program that uses a button to light a LED.
 * When the button is pressed the LED should be lit.
 * When the button is released the LED should go off.
 * Program another button to act as an on/off switch.
 * Pressing and releasing the button once should light the LED.
 * The next press/release cycle should turn the LED off.
 *
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

// Comment

#ifdef DEBUG
void
_error_(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

// Main function
int main(void)
{
    // Variable declaration
    unsigned char ucDelta, ucState;
    unsigned int lightMode = 0;

    //Pin+Buttons config
    PinoutSet(false, false);
    ButtonsInit();
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    while (1) {
        ucState = ButtonsPoll(&ucDelta, 0);

        // When left button is pressed and released we turn on/off the LED
        if (BUTTON_RELEASED(LEFT_BUTTON, ucState, ucDelta)) {
            if (lightMode == 0) lightMode = 1;
            else lightMode = 0;

            LEDWrite(CLP_D1, lightMode);
        }


        // As long a the right button is pressed it will turn on the LED, and as soon as is released it will turn off the LED again
        if (BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
            LEDWrite(CLP_D1, 1);

        if (BUTTON_RELEASED(RIGHT_BUTTON, ucState, ucDelta))
            if (lightMode == 0) LEDWrite(CLP_D1, 0);
    }

    return 0;
}
