#include <drivers/uart_driver.h>

void UART_init(uint32_t ui32Base)
{
    UART_module = ui32Base;
    /*
     * INITIALIZATION
     */

    // Configure UART module
    if (UART_module == 7)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R7;
    else if (UART_module == 6)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R6;
    else if (UART_module == 5)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5;
    else if (UART_module == 4)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R4;
    else if (UART_module == 3)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R3;
    else if (UART_module == 2)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R2;
    else if (UART_module == 1)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    else if (UART_module == 0)
        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

    // Enable the clock to the appropiate GPIO module
    if (UART_module == 0 || UART_module == 2 || UART_module == 3
            || UART_module == 4)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // port A
        // AFSEL, PCTL & Digital enable
        uint32_t afsel;
        uint32_t pctl;
        uint32_t den;
        if (UART_module == 0)
        {
            afsel = (1 << 0) | (1 << 1);
            pctl = (1 << 0) | (1 << 4);
            den = (1 << 0) | (1 << 1);
        }
        else if (UART_module == 2)
        {
            afsel = (1 << 6) | (1 << 7);
            pctl = (1 << 24) | (1 << 28);
            den = (1 << 6) | (1 << 7);
        }
        else if (UART_module == 3)
        {
            afsel = (1 << 4) | (1 << 5);
            pctl = (1 << 16) | (1 << 20);
            den = (1 << 4) | (1 << 5);
        }
        else if (UART_module == 4)
        {
            afsel = (1 << 2) | (1 << 3);
            pctl = (1 << 8) | (1 << 12);
            den = (1 << 2) | (1 << 3);
        }
        GPIO_PORTA_AHB_AFSEL_R = afsel;
        GPIO_PORTA_AHB_PCTL_R = pctl;
        GPIO_PORTA_AHB_DEN_R = den;
    }
    else if (UART_module == 1)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // port B
        // AFSEL
        GPIO_PORTB_AHB_AFSEL_R = (1 << 0) | (1 << 1);
        // PCTL
        GPIO_PORTB_AHB_PCTL_R = (1 << 0) | (1 << 4);
        // Digital enable
        GPIO_PORTB_AHB_DEN_R = (1 << 0) | (1 << 1);

    }
    else if (UART_module == 5 || UART_module == 7)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // port C
        // AFSEL, PCTL & digital enable
        uint32_t afsel;
        uint32_t pctl;
        uint32_t den;
        if (UART_module == 5)
        {
            afsel = (1 << 6) | (1 << 7);
            pctl = (1 << 28) | (1 << 24);
            den = (1 << 6) | (1 << 7);
        }
        else if (UART_module == 7)
        {
            afsel = (1 << 4) | (1 << 5);
            pctl = (1 << 16) | (1 << 20);
            den = (1 << 4) | (1 << 5);
        }
        GPIO_PORTC_AHB_AFSEL_R = afsel;
        GPIO_PORTC_AHB_PCTL_R = pctl;
        GPIO_PORTC_AHB_DEN_R = den;
    }
    else if (UART_module == 6)
    {
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R13; // port P
        // AFSEL
        GPIO_PORTP_AFSEL_R = (1 << 1) | (1 << 0);
        // PCTL
        GPIO_PORTP_PCTL_R = (1 << 0) | (1 << 4);
        // Digital enable
        GPIO_PORTP_DEN_R = (1 << 1) | (1 << 0);
    }

    /*
     * CONFIGURATION
     */
    // Get baud-rate divisor
    // brd = 16000000/ (16 * 9600) = 104.1666
    // uartfbrd(divfrac) = integer(0.1666 * 64 + 0.5) = 11
    uint32_t uartibrd = 104;
    uint32_t uartfbrd = 11;

    // Configure serial parameters
    uint32_t lcrh = (0x3 << 5);
    // Charge UARTIBRD, UARTFBRD and UARTLCRH
    if (UART_module == 7)
    {
        UART7_CTL_R &= ~(1 << 0);
        UART7_IBRD_R = uartibrd;
        UART7_FBRD_R = uartfbrd;
        UART7_LCRH_R = lcrh;
        UART7_CC_R = 0x0;
        UART7_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 6)
    {
        UART6_CTL_R &= ~(1 << 0);
        UART6_IBRD_R = uartibrd;
        UART6_FBRD_R = uartfbrd;
        UART6_LCRH_R = lcrh;
        UART6_CC_R = 0x0;
        UART6_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 5)
    {
        UART5_CTL_R &= ~(1 << 0);
        UART5_IBRD_R = uartibrd;
        UART5_FBRD_R = uartfbrd;
        UART5_LCRH_R = lcrh;
        UART5_CC_R = 0x0;
        UART5_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 4)
    {
        UART4_CTL_R &= ~(1 << 0);
        UART4_IBRD_R = uartibrd;
        UART4_FBRD_R = uartfbrd;
        UART4_LCRH_R = lcrh;
        UART4_CC_R = 0x0;
        UART4_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 3)
    {
        UART3_CTL_R &= ~(1 << 0);
        UART3_IBRD_R = uartibrd;
        UART3_FBRD_R = uartfbrd;
        UART3_LCRH_R = lcrh;
        UART3_CC_R = 0x0;
        UART3_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 2)
    {
        UART2_CTL_R &= ~(1 << 0);
        UART2_IBRD_R = uartibrd;
        UART2_FBRD_R = uartfbrd;
        UART2_LCRH_R = lcrh;
        UART2_CC_R = 0x0;
        UART2_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 1)
    {
        UART1_CTL_R &= ~(1 << 0);
        UART1_IBRD_R = uartibrd;
        UART1_FBRD_R = uartfbrd;
        UART1_LCRH_R = lcrh;
        UART1_CC_R = 0x0;
        UART1_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }
    else if (UART_module == 0)
    {
        UART0_CTL_R &= ~(1 << 0);
        UART0_IBRD_R = uartibrd;
        UART0_FBRD_R = uartfbrd;
        UART0_LCRH_R = lcrh;
        UART0_CC_R = 0x0;
        UART0_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);
    }

    UART_enable = 1; // Mark the UART as it has been init. and config.
}

char UART_getChar()
{
    // If the UART has been initialized
    if (UART_enable)
    {
        char c;
        if (UART_module == 0)
        {
            while ((UART0_FR_R & (1 << 4)) != 0)    // Wait until is ready to get the data
                ;
            c = UART0_DR_R; // Gets the data
        }
        else if (UART_module == 1)
        {
            while ((UART1_FR_R & (1 << 4)) != 0)
                ;
            c = UART1_DR_R;
        }
        else if (UART_module == 2)
        {
            while ((UART2_FR_R & (1 << 4)) != 0)
                ;
            c = UART2_DR_R;
        }
        else if (UART_module == 3)
        {
            while ((UART3_FR_R & (1 << 4)) != 0)
                ;
            c = UART3_DR_R;
        }
        else if (UART_module == 4)
        {
            while ((UART4_FR_R & (1 << 4)) != 0)
                ;
            c = UART4_DR_R;
        }
        else if (UART_module == 5)
        {
            while ((UART5_FR_R & (1 << 4)) != 0)
                ;
            c = UART5_DR_R;
        }
        else if (UART_module == 6)
        {
            while ((UART6_FR_R & (1 << 4)) != 0)
                ;
            c = UART6_DR_R;
        }
        else if (UART_module == 7)
        {
            while ((UART7_FR_R & (1 << 4)) != 0)
                ;
            c = UART7_DR_R;
        }
        return c;
    }
}

void UART_putChar(char c)
{
    // If the UART has been initialized
    if (UART_enable)
    {
        if (UART_module == 0)
        {
            while ((UART0_FR_R & (1 << 5)) != 0)    // If the UART is ready to get the data
                ;
            UART0_DR_R = c; // The data is put in the register
        }
        else if (UART_module == 1)
        {
            while ((UART1_FR_R & (1 << 5)) != 0)
                ;
            UART1_DR_R = c;
        }
        else if (UART_module == 2)
        {
            while ((UART2_FR_R & (1 << 5)) != 0)
                ;
            UART2_DR_R = c;
        }
        else if (UART_module == 3)
        {
            while ((UART3_FR_R & (1 << 5)) != 0)
                ;
            UART3_DR_R = c;
        }
        else if (UART_module == 4)
        {
            while ((UART4_FR_R & (1 << 5)) != 0)
                ;
            UART4_DR_R = c;
        }
        else if (UART_module == 5)
        {
            while ((UART5_FR_R & (1 << 5)) != 0)
                ;
            UART5_DR_R = c;
        }
        else if (UART_module == 6)
        {
            while ((UART6_FR_R & (1 << 5)) != 0)
                ;
            UART6_DR_R = c;
        }
        else if (UART_module == 7)
        {
            while ((UART7_FR_R & (1 << 5)) != 0)
                ;
            UART7_DR_R = c;
        }

    }

}

void UART_reset()
{
//    RCGCUART reset 0x0000.0000        # Universal Asynchronous Receiver/Transmitter Run Mode Clock Gating Control
//    RCGCGPIO reset 0x0000.0000        # General-Purpose Input/Output Run Mode Clock Gating Control
//    GPIOAFSEL reset 0x0000.0000 (for GPIO ports that are not listed in attached table     # GPIO Alternate Function Select
//    GPIOPCTL reset 0x0000.0000 (for GPIO ports that are not listed in attached table      # GPIO Port Control
//    UARTCTL reset 0x0000.0300         # UART Control
//    UARTIBRD all bits cleared on reset# UART Integer Baud-Rate Divisor
//    UARTFBRD all bits cleared on reset# UART Fractional Baud-Rate Divisor
//    UARTLCRH reset 0x0000.0000      # UART Line Control
//    UARTCC reset 0x0000.0000        # UART Clock Configuration

    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    GPIO_PORTA_AHB_AFSEL_R = 0x0;
    GPIO_PORTA_AHB_PCTL_R = 0x0;
    GPIO_PORTA_AHB_DEN_R = 0X0;
    UART0_CTL_R = 0X300;
    UART0_IBRD_R = 0x0;
    UART0_FBRD_R = 0X0;
    UART0_LCRH_R = 0X0;
    UART0_CC_R = 0X0;
}

void UART_putString(char* string)
{
    while (*string) // While the pointer doesn't point to the null character
    {
        UART_putChar(*(string++));  // Prints the character and move the pointer to the next one
    }
}

char* UART_getString()
{
    char string[50];
    char* final_string = string;
    char c;
    int i;
    // Iterating until receiving the enter character or the string gets to its maximum
    for (i = 0; i < 49; i++)
    {
        c = UART_getChar(); // Gets the character from the terminal
        // It breaks if the character is one of the following ones
        if (c == 13 || c == 10)
        {
            string[i] = 0;
            break;
        }
        UART_putChar(c);
        string[i] = c;  // Concatenate the char. to the array
        if (i == 48) string[i+1] = 0;
    }
    return final_string;
}
