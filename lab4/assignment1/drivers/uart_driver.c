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
    /*
     * INITIALIZATION
     */

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
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // port A
        // AFSEL
        GPIO_PORTA_AHB_AFSEL_R |= 0xFF;
        // PCTL
        uint32_t pctl;
        if (ui32Base == 0)
        {
            pctl = (1 << 0) | (1 << 4);
        }
        else if (ui32Base == 2)
        {
            pctl = (1 << 24) | (1 << 28);
        }
        else if (ui32Base == 3)
        {
            pctl = (1 << 16) | (1 << 20);
        }
        else if (ui32Base == 4)
        {
            pctl = (1 << 8) | (1 << 12);
        }
        GPIO_PORTA_AHB_PCTL_R |= pctl;
    }
    else if (ui32Base == 1)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // port B
        // AFSEL
        GPIO_PORTB_AHB_AFSEL_R |= 0xFF;
        // PCTL
        GPIO_PORTB_AHB_PCTL_R = (1 << 0) | (1 << 4);
    }
    else if (ui32Base == 5 || ui32Base == 7)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // port C
        // AFSEL
        GPIO_PORTC_AHB_AFSEL_R |= 0xFF;

        // PCTL
        uint32_t pctl;
        if (ui32Base == 5)
        {
            pctl = (1 << 28) | (1 << 24);
        }
        else if (ui32Base == 7)
        {
            pctl = (1 << 16) | (1 << 20);
        }
        GPIO_PORTC_AHB_PCTL_R |= pctl;
    }
    else if (ui32Base == 6)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R13; // port P
        // AFSEL
        GPIO_PORTP_AHB_AFSEL_R |= 0xFF;
        // PCTL
        GPIO_PORTP_PCTL_R |= (1 << 0) | (1 << 4);
        // Digital enable
        GPIO_PORTP_AHB_DEN_R = (1 << 1) | (1 << 0);
    }

    /*
     * CONFIGURATION
     */
    // Get baud-rate divisor
    // brd = 16000000/ (16 * 9600) = 104.1666
    // uartfbrd(divfrac) = integer(0.1666 * 64 + 0.5) = 11
    uint32_t uartibrd = 104;
    uint32_t uartfbrd = 11;

    // Disable UART
    UART_CTL_UARTEN = 0;
    // Configure serial parameters
    uint32_t lcrh = (0x3 << 5);
    // Charge UARTIBRD, UARTFBRD and UARTLCRH
    if (ui32Base == 7)
    {
        UART7_CTL_R &= ~(1<<0);
        UART7_IBRD_R |= uartibrd;
        UART7_FBRD_R |= uartfbrd;
        UART7_LCRH_R |= lchr;
        UART7_CC_R &= ~(0xF);
        UART7_CTL_R |= 1;
    }
    else if (ui32Base == 6)
    {
        UART6_CTL_R &= ~(1<<0);
        UART6_IBRD_R |= uartibrd;
        UART6_FBRD_R |= uartfbrd;
        UART6_LCRH_R |= lchr;
        UART6_CC_R &= ~(0xF);
        UART6_CTL_R |= 1;
    }
    else if (ui32Base == 5)
    {
        UART5_CTL_R &= ~(1<<0);
        UART5_IBRD_R |= uartibrd;
        UART5_FBRD_R |= uartfbrd;
        UART5_LCRH_R |= lchr;
        UART5_CC_R &= ~(0xF);
        UART5_CTL_R |= 1;
    }
    else if (ui32Base == 4)
    {
        UART4_CTL_R &= ~(1<<0);
        UART4_IBRD_R |= uartibrd;
        UART4_FBRD_R |= uartfbrd;
        UART4_LCRH_R |= lchr;
        UART4_CC_R &= ~(0xF);
        UART4_CTL_R |= 1;
    }
    else if (ui32Base == 3)
    {
        UART3_CTL_R &= ~(1<<0);
        UART3_IBRD_R |= uartibrd;
        UART3_FBRD_R |= uartfbrd;
        UART3_LCRH_R |= lchr;
        UART3_CC_R &= ~(0xF);
        UART3_CTL_R |= 1;
    }
    else if (ui32Base == 2)
    {
        UART2_CTL_R &= ~(1<<0);
        UART2_IBRD_R |= uartibrd;
        UART2_FBRD_R |= uartfbrd;
        UART2_LCRH_R |= lchr;
        UART2_CC_R &= ~(0xF);
        UART2_CTL_R |= 1;
    }
    else if (ui32Base == 1)
    {
        UART1_CTL_R &= ~(1<<0);
        UART1_IBRD_R |= uartibrd;
        UART1_FBRD_R |= uartfbrd;
        UART1_LCRH_R |= lchr;
        UART1_CC_R &= ~(0xF);
        UART1_CTL_R |= 1;
    }
    else if (ui32Base == 0)
    {
        UART0_CTL_R &= ~(1<<0);
        UART0_IBRD_R |= uartibrd;
        UART0_FBRD_R |= uartfbrd;
        UART0_LCRH_R |= lchr;
        UART0_CC_R &= ~(0xF);
        UART0_CTL_R |= 1;
    }
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
