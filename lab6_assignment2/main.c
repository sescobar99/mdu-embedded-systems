#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
static volatile char empty[BUFFER_SIZE];

static volatile char* reverseLineFeed = "\033[A";
static volatile int inputCount = 0;
static volatile int lastInputCountValue = -1;

static volatile SemaphoreHandle_t status_task_sem;

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

void configureButtons(void)
{
    ButtonsInit();
}

// int to string
void itos(int number, char* str)
{
    const int size = 10;
    int digits[size];
    int i = 0, e = 0;
    while (1)
    {
        digits[i++] = number % 10;
        number /= 10;
        if (number == 0)
            break;
    }
    i -= 1;
    while (i > -1)
    {
        str[e++] = digits[i--] + '0';
    }
    str[e] = NULL_CHARACTER;
}

// Shift all the values to the left and inserts the new element
// to the las postion, that will be empty
void addElement(char str[], int length, char newElement)
{
    int i;
    for (i = 0; i < length - 2; i++)
    {
        str[i] = str[i + 1];
    }
    str[length - 2] = newElement;
    str[length - 1] = NULL_CHARACTER;
}

void printString(char* str)
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
        printString(buffer); // Prints the letters
        while (!UARTCharsAvail(UART0_BASE)) // Waits to get obtain a char input
        {
        }
        inputChar = UARTCharGetNonBlocking(UART0_BASE); // Gets the char input
        inputCount++; // Increase the chars counter
        addElement(buffer, BUFFER_SIZE, inputChar); // Add the char to the end of the buffer
    }
}

/*
 * When buttons is pressed this function gets called and prints continously the number
 * of times a key has been pressed.
 */
void vStatusTask(void* pvParameters)
{
    char str[10];
    while (1)
    {
        xSemaphoreTake(status_task_sem, portMAX_DELAY);
        if (inputCount != lastInputCountValue)
        {
            lastInputCountValue = inputCount;

            printString("\n");
            itos(inputCount, str);
            printString(str);
            printString(reverseLineFeed);
        }
        xSemaphoreGive(status_task_sem);
    }
}


/*
 * Continuously polls buttons. If pressed gives semaphore (allowing status task to print) and delays for 10 seconds
 * Takes the semaphore again and continues pooling.
 */
void vActivateAuxTask(void* pvParameters)
{
    unsigned char ucDelta, ucState;
    while (1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        //check both buttons
        if ((GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x1) == 0
                || (GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x2)
                        == 0)
        {
            xSemaphoreGive(status_task_sem);
            lastInputCountValue = -1;
            vTaskDelay(pdMS_TO_TICKS(10000));
            xSemaphoreTake(status_task_sem, portMAX_DELAY);
            printString("\n");
            printString(empty);
            printString(reverseLineFeed);
        }
    }

}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create tasks
    xTaskCreate(vPrintLastChars, "UART", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vStatusTask, "STATUS TASK", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vActivateAuxTask, "ACTIVATION", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    // Create semaphore
    status_task_sem = xSemaphoreCreateCounting(1, 0);

    int i;
    //Init arrays.  Empty is used for clearing the screen.
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        empty[i] = SPACE;
        buffer[i] = SPACE;
    }
    empty[BUFFER_SIZE - 1] = NULL_CHARACTER;
    buffer[BUFFER_SIZE - 1] = NULL_CHARACTER;

    configureButtons(); // Configure buttons
    configureUART(); // Init UART
    vScheduling(); // Start scheduler

}
