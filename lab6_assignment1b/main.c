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

#define BUFFER_SIZE 10
#define CONSUMERS_NUMBER 5

static volatile SemaphoreHandle_t producer_sem, consumer_sem, buffer_sem;

static volatile unsigned int buffer[BUFFER_SIZE], top, bottom;
static volatile unsigned int consumers[CONSUMERS_NUMBER];

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

void printString(char* str, ...)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, 13);
}

unsigned int produce()
{
    static unsigned int counter = 0;
    printString("PRODUCER: Starts production\n");
    vTaskDelay(pdMS_TO_TICKS(200));
    printString("PRODUCER: Produces a value\n");
    return ++counter;
}

void consume()
{
    printString("A CONSUMER: Starts consuming a value\n");
    vTaskDelay(pdMS_TO_TICKS(3000));
    printString("A CONSUMER: Finishes consuming a value\n");
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

        xSemaphoreTake(producer_sem, portMAX_DELAY); // Wait until there is empty spaces in buffer
        buffer[top] = produced_val; // Insert the value
        printString("PRODUCER: Puts a value in the buffer\n");
        top = (top + 1) % BUFFER_SIZE;

        xSemaphoreGive(consumer_sem); // Give permission to some consumer to keep consuming
    }
}
// CONSUMER
void vTaskConsumer(void* pvParameters)
{
    unsigned int value_to_consume;
    while (1)
    {
        xSemaphoreTake(consumer_sem, portMAX_DELAY); // Wait until there is elements in buffer

        xSemaphoreTake(buffer_sem, portMAX_DELAY); // Waits to access the consumers shared resource
        value_to_consume = buffer[bottom]; // Take the value
        bottom = (bottom + 1) % BUFFER_SIZE; // Move pointer to first in buffer
        printString("A CONSUMER: Taks a value from the buffer\n");
        xSemaphoreGive(buffer_sem);

        xSemaphoreGive(producer_sem); // Give permission to producer to keep producing

        consume(); // Consume
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create tasks
    static unsigned char ucParameterToPass;

    xTaskCreate(vTaskProducer, "PRODUCER", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);

    unsigned int i;
    for (i = 0; i < CONSUMERS_NUMBER; i++)
    {
        consumers[i] = i;
        xTaskCreate(vTaskConsumer, "CONSUMER", configMINIMAL_STACK_SIZE,
                    (void* ) consumers[i], tskIDLE_PRIORITY + 1, NULL);
    }

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    top = 0;
    bottom = top;

    // Create semaphores
    producer_sem = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    consumer_sem = xSemaphoreCreateCounting(5, 0);
    buffer_sem = xSemaphoreCreateBinary();

    xSemaphoreGive(buffer_sem); // Initialize the binary semaphore

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }
}
