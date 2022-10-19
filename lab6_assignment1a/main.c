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

static volatile SemaphoreHandle_t producer_sem, consumer_sem, buffer_sem,
        debug_sem;

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

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
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

        xSemaphoreTake(producer_sem, portMAX_DELAY);

        buffer[top] = produced_val;
        printString("PRODUCER: Puts a value in the buffer\n");
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

        xSemaphoreTake(buffer_sem, portMAX_DELAY);
        value_to_consume = buffer[bottom];
        bottom = (bottom + 1) % BUFFER_SIZE;
        printString("A CONSUMER: Taks a value from the buffer\n");
        xSemaphoreGive(buffer_sem);

        xSemaphoreGive(producer_sem);

        consume();
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
    debug_sem = xSemaphoreCreateBinary();

    xSemaphoreGive(buffer_sem);
    xSemaphoreGive(debug_sem);

    configureUART(); // Init UART
    vScheduling(); // Start scheduler

    while (1)
    {
    }
}
