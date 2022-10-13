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
    UART_reset();
    UART_init(6);
    UART_putChar('n');
    while (1) {
        UART_putString(UART_getString());
    }
}
