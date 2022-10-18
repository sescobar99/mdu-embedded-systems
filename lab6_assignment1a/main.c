#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/tm4c129encpdt.h"
#include <string.h>

static volatile SemaphoreHandle_t bin_sem;

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

/

//*******************************************************
// Tasks code
//*******************************************************
// PRODUCER
void vTaskProducer(void* pvParameters)
{
    while (1)
    {

    }
}
// CONSUMER
void vTaskConsumer(void* pvParameters)
{
    while (1)
    {
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{

    // Create tasks
    static unsigned char ucParameterToPass1, ucParameterToPass2;
    xTaskHandle xHandle1, xHandle2;

    xTaskCreate(vTaskA, "PRODUCER", configMINIMAL_STACK_SIZE, &ucParameterToPass1,
                tskIDLE_PRIORITY + 1, &xHandle1);
    xTaskCreate(vTaskB, "CONSUMER", configMINIMAL_STACK_SIZE, &ucParameterToPass2,
                tskIDLE_PRIORITY + 1, &xHandle2);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    configureUART();
    vScheduling();

    while (1)
    {
    }
}
