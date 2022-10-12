/**
 * MDU ES Lab4 Assignment1
 * Implement UART driver in polling mode using "Direct Register Access Model"
 * The LED should start blinking after you press the button.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */

#include "drivers/uart_driver.h"
#include <stdbool.h>
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include <stdio.h>
#include "drivers/pinout.h"
#include "inc/hw_memmap.h"

int main(void)
{
    UART_init(0);
    while (1) {
        UART_getChar();

    }
}
