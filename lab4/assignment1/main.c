#include "drivers/uart_driver.h"

int main(void)
{
    UART_reset();
    UART_init(0);
    UART_putChar('n');
    while (1)
    {
    }
}
