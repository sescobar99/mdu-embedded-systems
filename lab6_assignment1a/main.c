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

void printString(char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, 13);
}

void idleWork(int seconds)
{
    TickType_t currentTick = xTaskGetTickCount();
    while (xTaskGetTickCount() - currentTick < pdMS_TO_TICKS(seconds * 1000))
    {
    }
}

void deadlineMiss(TickType_t xLastWakeTime, TickType_t period, char cTask)
{
    TickType_t xPeriodLimit = xLastWakeTime + period;
    TickType_t xActualTime = xTaskGetTickCount();
    if (xPeriodLimit < xActualTime)
    {
        printString("DEADLINE MISSED\n");
    }
    else
    {
        vTaskDelayUntil(&xLastWakeTime, period);
    }

}

unsigned int produce()
{
    static unsigned int counter = 0;
    printString("PRODUCER: Starts production\n");
    printString("Before\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    printString("After\n");
    char str[50];
    sprintf(str, "PRODUCER: Produces value %u\n", counter+1);
    printString(str);
    return ++counter;
}

void consume(unsigned int value_to_consume, unsigned int task)
{
    char str[50];
    sprintf(str, "CONSUMER %u: Starts consuming value %u\n", task,
            value_to_consume);
    printString(str);
    vTaskDelay(pdMS_TO_TICKS(5000));
    sprintf(str, "CONSUMER %u: Finishes consuming value %u\n", task,
            value_to_consume);
    printString(str);
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

        xSemaphoreTake(producer_sem, portMAX_DELAY);

        buffer[top] = produced_val;
        char str[50];
        sprintf(str, "PRODUCER: Put value %u in buffer\n", produced_val);
        printString(str);
        top = (top + 1) % BUFFER_SIZE;

        xSemaphoreGive(consumer_sem);
    }
}
// CONSUMER
void vTaskConsumer(void* pvParameters)
{
    unsigned int value_to_consume;
    while (1)
    {
        xSemaphoreTake(consumer_sem, portMAX_DELAY);

        //xSemaphoreTake(buffer_sem, portMAX_DELAY);
        printString("He entrado3\n");
        value_to_consume = buffer[bottom];
        bottom = (bottom + 1) % BUFFER_SIZE;
        char str[50];
        sprintf(str, "CONSUMER %u: Takes value %u from buffer\n", *(unsigned int*) pvParameters, value_to_consume);
        printString(str);
//        xSemaphoreGive(buffer_sem);

        xSemaphoreGive(producer_sem);

        consume(value_to_consume, *(unsigned int*) pvParameters);
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
                &ucParameterToPass, tskIDLE_PRIORITY + 1, NULL);

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

    xSemaphoreGive(buffer_sem);

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }
}
