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

static volatile SemaphoreHandle_t producer_sem, consumer_sem;

static volatile unsigned int buffer;

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
    printString("PRODUCER: Starts production\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    char str[50];
    sprintf(str, "PRODUCER: Produces value %u\n", counter);
    printString(str);
    return ++counter;
}

// Consume a value during five seconds
void consume(unsigned int value_to_consume)
{
    char str[50];
    sprintf(str, "CONSUMER: Starts consuming value %u\n", value_to_consume);
    printString(str);
    vTaskDelay(pdMS_TO_TICKS(5000));
    sprintf(str, "CONSUMER: Finishes consuming value %u\n", value_to_consume);
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

        // Checking the buffer with polling technique
        while (buffer != 0)
            vTaskDelay(pdMS_TO_TICKS(100));

        buffer = produced_val;
    }
}
// CONSUMER
void vTaskConsumer(void* pvParameters)
{
    unsigned int value_to_consume;
    while (1)
    {
        // Checking the buffer with polling technique
        while (buffer == 0)
            vTaskDelay(pdMS_TO_TICKS(100));

        value_to_consume = buffer;
        buffer = 0;
        consume(value_to_consume); // Consume
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create tasks
    xTaskCreate(vTaskProducer, "PRODUCER", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTaskConsumer, "CONSUMER", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    buffer = 0; // Init buffer

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }
}
