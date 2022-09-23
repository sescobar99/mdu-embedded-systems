
/**
 * MDU ES Lab1 Assignment1
 * Button input. Write a program to start blinking LEDs as an action of button events.
 * The LED should start blinking after you press the button.
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
    unsigned int activated = 0; // indicates when the blinking mode is on
    unsigned int lightMode = 1; // on/off
    unsigned int counter = 0; //Counter will help us to do idle cycles
    unsigned int idleCycles = 10000;
    volatile uint32_t ui32Loop;


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
     // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){
    }

    PinoutSet(false, false);
    ButtonsInit();
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    while (1) {
        ucState = ButtonsPoll(&ucDelta, 0);

        //Check if we should turn on/off the blinking
        if (BUTTON_RELEASED(RIGHT_BUTTON, ucState, ucDelta)) {
            activated = activated ? 0 : 1;
        }

        if (activated && counter == 0) {
            LEDWrite(CLP_D1, lightMode);
            lightMode = lightMode ? 0 : 1;
            counter++;
        } else if (counter > 0) {
            counter = (counter >= idleCycles ? 0 : counter + 1);
        } else { // !activated
            LEDWrite(CLP_D1, 0);
            counter = 0;
        }
    }

    return 0;
}
