/**
 * MDU ES Lab4 Assignment1
 * Implement UART driver in polling mode using "Direct Register Access Model"
 * The LED should start blinking after you press the button.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */

#include "drivers/uart_driver.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/tm4c129encpdt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

int main(void)
{
    UART_init(0);
    while (1) {
        UART_putChar('h');
    }
}
