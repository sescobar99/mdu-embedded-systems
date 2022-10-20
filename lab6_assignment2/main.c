#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

#define BUFFER_SIZE 16 //Creates 15 position char array + null terminated

#define NULL_CHARACTER 0
#define BACKSPACE 8
#define LINE_FEED 10
#define CARRIAGE_RETURN 13
#define SPACE 32
#define DELETE 127

static volatile char buffer[BUFFER_SIZE];
static volatile char erase[BUFFER_SIZE];

static volatile char* reverseLineFeed = "\033[A";
static volatile int inputCount = 0;

void configureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTConfigSetExpClk(UART0_BASE, 16000000, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));
}

void addElement(char str[], int lenght, char newElement)
{
    int i;
    for (i = 0; i < lenght - 2; i++)
    {
        str[i] = str[i + 1];
    }
    str[lenght - 2] = newElement;
    str[lenght - 1] = NULL_CHARACTER;
    return;
}

void printString(char* str, ...)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, CARRIAGE_RETURN);
//    UARTCharPut(UART0_BASE, LINE_FEED);
}

//*******************************************************
// Tasks code
//*******************************************************

/*
 * Prints last BUFFER_SIZE-1 chars stored in the buffer.
 * Waits for next inputs and updates the buffer
 */
void vPrintLastChars(void* pvParameters)
{
    char inputChar;
    while (1)
    {
        printString(buffer);
        printString("ab\n");
        printString(reverseLineFeed);
        printString("c\n");

        while (!UARTCharsAvail(UART0_BASE))
        {
        }
        inputChar = UARTCharGetNonBlocking(UART0_BASE);
        inputCount++;
        addElement(buffer, BUFFER_SIZE, inputChar);
//        if(inputChar == )

    }
}


void vCheckButtons(void* pvParameters)
{
    unsigned char ucDelta, ucState;
    while (1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        //check both buttons
        if ((GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x1) == 0 || (GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x2) == 0)
        {
//            left_pressed = 1;
            LEDWrite(CLP_D1, 1);
            vTaskDelay(pdMS_TO_TICKS(10000));
            LEDWrite(CLP_D1, 0);
//            left_pressed = 0;
        }
    }
}



//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create tasks

    xTaskCreate(vPrintLastChars, "UART", configMINIMAL_STACK_SIZE,
                (void * ) BUFFER_SIZE, tskIDLE_PRIORITY + 1, NULL);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    int i;
    //Init arrays
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        erase[i] = DELETE;
        buffer[i] = SPACE;
    }
    erase[BUFFER_SIZE - 1] = NULL_CHARACTER;
    buffer[BUFFER_SIZE - 1] = NULL_CHARACTER;

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }

}
