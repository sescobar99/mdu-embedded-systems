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
static volatile char erase[BUFFER_SIZE];

static volatile char* reverseLineFeed = "\033[A";
static volatile int inputCount = 0;

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
char* itos (int number)
{
    const int size = 10;
    int digits[size];
    int i = 0, e;
    while (1)
    {
        digits[i++] = number % 10;
        number /= 10;
        if (number == 0) break;
    }
    char* str = (char*) calloc(i, sizeof(char));
    for (e = 0; i > 0; i--, e++)
    {
        *(str + e) = digits[i-1] + '0';
    }
    return str;

}

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
        printString(buffer);
        while (!UARTCharsAvail(UART0_BASE))
        {
        }
        inputChar = UARTCharGetNonBlocking(UART0_BASE);
        inputCount++;
        addElement(buffer, BUFFER_SIZE, inputChar);
    }
}

void vStatusTask(void* pvParameters)
{
    while (1)
    {
        xSemaphoreTake(status_task_sem, portMAX_DELAY);
        printString(itos(inputCount));
        xSemaphoreGive(status_task_sem);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

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
            vTaskDelay(pdMS_TO_TICKS(10000));
            xSemaphoreTake(status_task_sem, portMAX_DELAY);
            printString("TENGO SEMAFOROOOO\n");
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
    status_task_sem = xSemaphoreCreateCounting(1, 0);

    int i;
    //Init arrays
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        erase[i] = DELETE;
        buffer[i] = SPACE;
    }
    erase[BUFFER_SIZE - 1] = NULL_CHARACTER;
    buffer[BUFFER_SIZE - 1] = NULL_CHARACTER;

    configureButtons();
    configureUART(); // Init UART
    vScheduling(); // Start scheduler

}
