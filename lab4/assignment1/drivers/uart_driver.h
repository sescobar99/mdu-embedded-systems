#include "inc/tm4c129encpdt.h"
#include <stdint.h>

static volatile uint32_t UART_module;
static volatile uint8_t UART_enable = 0;

/**
 * This  Function  initializes  the  UART  driver,  here  we  set  up  the  hardware module.
 */
void UART_init(uint32_t ui32Base);

/**
 * This Function is used to receive one character.
 */
char UART_getChar();

/**
 * This function is used to transmit one character.
 */
void UART_putChar(char c);


/**
 * This  function  resets  the  driver  to  a  save  state
 * (reset  all  registers  that  could lead to unpredictable behavior).
 */
void UART_reset();

/**
 * This function uses the putChar function to write a string.
 */
void UART_putString(char* string);

/**
 * This function uses the getChar function to read a string.
 */
char* UART_getString();





