/**
 * MDU ES Lab4 Assignment1
 * Implement UART driver in polling mode using "Direct Register Access Model"
 * The LED should start blinking after you press the button.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */

#include "drivers/uart_driver.h"

int main(void)
{
    UART_init(0);
    while (1) {
        UARTprintf("Hola mundo\n");
    }
}
