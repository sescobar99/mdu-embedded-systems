#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/tm4c129encpdt.h"
#include <string.h>

#define BUFFER_SIZE 100

static volatile TaskHandle_t producerHandle, consumerHandle;

static volatile unsigned int buffer[BUFFER_SIZE];
static volatile unsigned int top, bottom, bufferCounter;

static volatile unsigned int debugVal;

static volatile char string[100];

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

void printString(char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, 13);
}

// Produce a value during a second
unsigned int produce()
{
    static unsigned int counter = 0;

//    printString("PRODUCER: Starts production\n");
//    vTaskDelay(pdMS_TO_TICKS(50));
//    char str[50];
//    sprintf(str, "PRODUCER: Produces value %u\n", counter);
//    printString(str);
    return ++counter;
}

// Consume a value during five seconds
void consume(unsigned int value_to_consume)
{
    debugVal++;
//    char str[50];
//    sprintf(str, "CONSUMER: Starts consuming value %u\n", value_to_consume);
//    printString(str);
//    vTaskDelay(pdMS_TO_TICKS(50));
//    sprintf(str, "CONSUMER: Finishes consuming value %u\n", value_to_consume);
//    printString(str);
}

//*******************************************************
// Tasks code
//*******************************************************
// PRODUCER
void vTaskProducer(void* pvParameters)
{
    unsigned int produced_val;
    while (1)
    {
        produced_val = produce(); // Produce

        if (bufferCounter == BUFFER_SIZE)
            vTaskSuspend(producerHandle);

        buffer[top] = produced_val; // Insert the value
//        sprintf(string, "PRODUCER  : Puts %d in the buffer\n", produced_val);
//        printString(string);
        top = (top + 1) % BUFFER_SIZE;

        bufferCounter += 1;

        if (bufferCounter == 1)
            vTaskResume(consumerHandle);
    }
}
// CONSUMER
void vTaskConsumer(void* pvParameters)
{
    unsigned int value_to_consume;
    while (1)
    {
        if (bufferCounter == 0)
            vTaskSuspend(consumerHandle);

        value_to_consume = buffer[bottom]; // Take the value
        bottom = (bottom + 1) % BUFFER_SIZE; // Move pointer to first in buffer
//        sprintf(string, "CONSUMER: Takes %d from the buffer\n",
//                value_to_consume);
//        printString(string);

        bufferCounter -= 1;

        if (bufferCounter == BUFFER_SIZE - 1)
            vTaskResume(producerHandle);

        consume(value_to_consume); // Consume
    }
}
void vTaskDebug(void* pvParameters)
{
    unsigned int value_to_consume;
    int lastDebugVal = 0;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        if (lastDebugVal < debugVal)
        {
            printString("NOT DEADLOCK\n");
            lastDebugVal = debugVal;
        }
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    debugVal = 0;
    top = 0;
    bottom = top;
    bufferCounter = 0;

    // Create tasks
    xTaskCreate(vTaskProducer, "PRODUCER", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, &producerHandle);
    xTaskCreate(vTaskConsumer, "CONSUMER", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, &consumerHandle);
    xTaskCreate(vTaskDebug, "DEBUG", configMINIMAL_STACK_SIZE, NULL,
                    tskIDLE_PRIORITY + 3, NULL);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }
}
