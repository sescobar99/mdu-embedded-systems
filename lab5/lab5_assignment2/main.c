#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"


#define DEBUG 1
#define TIME_SIZE 1000

//*******************************************************
// Debug (UART)
//*******************************************************
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

static const unsigned int task_A_period = pdMS_TO_TICKS(10 * TIME_SIZE);
static const unsigned int task_B_period = pdMS_TO_TICKS(20 * TIME_SIZE);
static const unsigned int task_C_period = pdMS_TO_TICKS(20 * TIME_SIZE);

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

void idleWork(int seconds, char* pcTask)
{
    TickType_t currentTick = xTaskGetTickCount();
#if DEBUG
    char str[50];
    sprintf(str, "Task %s: WORKLOAD", pcTask);
    int i = 0;
    printString("\n");
#endif
    while (xTaskGetTickCount() - currentTick < pdMS_TO_TICKS(seconds * TIME_SIZE))
    {
#if DEBUG
        if (i % 5000 * TIME_SIZE == 0)
        {
            printString(str);
        }
        i++;
#endif
    }
}

void deadlineMiss(TickType_t xLastWakeTime, TickType_t period, char* pcTask)
{
    TickType_t xPeriodLimit = xLastWakeTime + period;
    TickType_t xActualTime = xTaskGetTickCount();
    if (xPeriodLimit < xActualTime)
    {
        char str[50];

        unsigned int ms_miss = (xActualTime - xPeriodLimit) * portTICK_PERIOD_MS;

        sprintf(str, "Task %s: DEADLINE MISSED by %u ms \n", pcTask, ms_miss);
        printString(str);

    }
    else
    {
        vTaskDelayUntil(&xLastWakeTime, period);
    }

}

//*******************************************************
// Tasks code
//*******************************************************

// High priority task
void vTaskA(void* pvParameters)
{
    TickType_t xLastWakeTime, xPeriodLimit, xActualTime;
    vTaskDelay(pdMS_TO_TICKS(3 * TIME_SIZE));
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();

        printString("Task A: Started\n");
        idleWork(1, "A");
        // Enters cs
        xSemaphoreTake(bin_sem, portMAX_DELAY);
        printString("Task A: sem take\n");
        int i;
        for (i = 0; i < 2; i++)
        {
            idleWork(1, "A");
        }
        printString("Task A: sem give\n");
        xSemaphoreGive(bin_sem);
        // Leaves cs
        idleWork(1, "A");
        printString("Task A: Finished\n");

        deadlineMiss(xLastWakeTime, task_A_period, "A");
    }
}
// Medium priority task
void vTaskB(void* pvParameters)
{
    TickType_t xLastWakeTime, xPeriodLimit, xActualTime;
    vTaskDelay(pdMS_TO_TICKS(6 * TIME_SIZE));
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();

        printString("Task B: Started\n");
        int i;
        for (i = 0; i < 4; i++)
        {
            idleWork(1, "B");
        }
        printString("Task B: Finished\n");

        deadlineMiss(xLastWakeTime, task_B_period, "B");
    }
}
// Low priority task
void vTaskC(void* pvParameters)
{
    TickType_t xLastWakeTime, xPeriodLimit, xActualTime;
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();

        printString("Task C: Started\n");
        idleWork(1, "C");
        // Enters cs
        xSemaphoreTake(bin_sem, portMAX_DELAY);
        printString("Task C: sem take\n");
        int i;
        for (i = 0; i < 6; i++)
        {
            idleWork(1, "C");
        }
        printString("Task C: sem give\n");
        xSemaphoreGive(bin_sem);
        // Leaves cs
        idleWork(1, "C");
        printString("Task C: Finished\n");

        deadlineMiss(xLastWakeTime, task_C_period, "C");
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create semaphore
    bin_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(bin_sem);

    // Create tasks
    static unsigned char ucParameterToPass1, ucParameterToPass2,
            ucParameterToPass3;
    xTaskHandle xHandle1, xHandle2, xHandle3;
    xTaskCreate(vTaskA, "TASKA", configMINIMAL_STACK_SIZE, &ucParameterToPass1,
                tskIDLE_PRIORITY + 3, &xHandle1);
    xTaskCreate(vTaskB, "TASKB", configMINIMAL_STACK_SIZE, &ucParameterToPass2,
                tskIDLE_PRIORITY + 2, &xHandle2);
    xTaskCreate(vTaskC, "TASKC", configMINIMAL_STACK_SIZE, &ucParameterToPass3,
                tskIDLE_PRIORITY + 1, &xHandle3);

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
