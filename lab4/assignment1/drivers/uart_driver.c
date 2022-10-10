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

void UART_init(uint32_t ui32Base){
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

}

char UART_getChar(){

}

void UART_putChar(char c){

}

void UART_reset(){

}
