#include "drivers/uart_driver.h"

int main(void)
{
    UART_reset();
    UART_init(0);
    while (1)
    {
        UART_putString(UART_getString());
    }
}
