#include <drivers/uart_driver.h>

//1. The driver shall have the following interface:
//2. The UART driver shall operate at 9600 baud. It is not necessary to make the baud rate
//configurable.
//3. The functions UART getChar() and UART putChar(char c) should not try to access the  hardware if the driver was not initialized first.
//4. A driver should be portable and only the interface functions should be accessible from other parts of the code.
//5. Packet length should be 8 bits.
//6. The driver should send no parity bit.
//7. The driver should send one stop bit.
//8. The driver should operate in normal channel mode.

void UART_init(uint32_t ui32Base)
{
//    Steps you need to do in the initialization:
//    1. Reset the driver to avoid unpredictable behavior during initialization.
//    2. Set the baud rate.
//    3. Set the message length.
//    4. Initialize the stop bit.
//    5. Set the driver to normal mode.
//    6. Enable the communication.
//    7. Enable the digital register bit for the GPIO pins, page 789
//     See the microcontroller datasheet section 19.4.
//    Note: The step 7 in the initialization in chapter 19 is missing

    // Configure UART module
    if (ui32Base == 7)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R7;
    else if (ui32Base == 6)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R6;
    else if (ui32Base == 5)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5;
    else if (ui32Base == 4)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R4;
    else if (ui32Base == 3)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R3;
    else if (ui32Base == 2)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;
    else if (ui32Base == 1)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    else if (ui32Base == 0)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

    // Enable the clock to the appropiate GPIO module
    if (ui32Base == 0 || ui32Base == 2 || ui32Base == 3 || ui32Base == 4)
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    else if (ui32Base == 0)
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    else if (ui32Base == 5 || ui32Base == 7)
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_R2;
    else if (ui32Base == 4)
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_R13;


}

char UART_getChar()
{

}

void UART_putChar(char c)
{

}

void UART_reset()
{

}
