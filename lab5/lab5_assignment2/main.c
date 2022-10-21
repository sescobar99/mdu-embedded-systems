/**
 * MDU ES Lab5 Assignment 2+3
 * 2.Implement three different tasks that have periods, release times and priorities in such a manner that the priority inversion takes place.
 * If done properly, the execution of a correctly implemented priority inversion will result in a deadline miss of the tasks.
 * To visualize the execution of the different tasks, you are  required to output certain keypoints to the UART.
 *
 * 3.Write a software function to detect the deadline miss.
 * Write the following information via the UART: which task misses its deadline and by how much time the deadline is missed.
 * All tasks should have this deadlines miss detection.
 *  Josep Naranjo
 *  &
 *  Santiago Escobar
 */
//*****************************************************************************
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#define PRINT_WORKLOAD 1
#define TIME_SIZE 1000 //1000 -> 1 time unit = 1 second
#define TASK_A_MISS_DEADLINE 1
#define TASK_B_MISS_DEADLINE 0
#define TASK_C_MISS_DEADLINE 0

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


//*******************************************************
// Tasks code
//*******************************************************
#if TASK_A_MISS_DEADLINE
static const unsigned int task_A_period = pdMS_TO_TICKS(10 * TIME_SIZE);
static const unsigned int task_B_period = pdMS_TO_TICKS(20 * TIME_SIZE);
static const unsigned int task_C_period = pdMS_TO_TICKS(20 * TIME_SIZE);
char *msg ="'A' will miss it´s deadline, because of the priority inversion\n";
#endif

#if TASK_B_MISS_DEADLINE
static const unsigned int task_A_period = pdMS_TO_TICKS(10 * TIME_SIZE);
static const unsigned int task_B_period = pdMS_TO_TICKS(2 * TIME_SIZE);
static const unsigned int task_C_period = pdMS_TO_TICKS(20 * TIME_SIZE);
char *msg = "'B' will miss it´s deadline, C will starve\n";
#endif

#if TASK_C_MISS_DEADLINE
static const unsigned int task_A_period = pdMS_TO_TICKS(10 * TIME_SIZE);
static const unsigned int task_B_period = pdMS_TO_TICKS(18 * TIME_SIZE);
static const unsigned int task_C_period = pdMS_TO_TICKS(2 * TIME_SIZE);
char *msg ="'A' & 'C' will miss deadlines\n";
#endif


//*******************************************************
// Helper functions
//*******************************************************

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


//Simulates work for the tasks
void idleWork(int seconds, char* pcTask)
{
    TickType_t currentTick = xTaskGetTickCount();
#if PRINT_WORKLOAD
    char str[50];
    printString("\n");
    sprintf(str, "\033[ATask %s: workload\n", pcTask);
    int i = 0;
#endif

    while (xTaskGetTickCount() - currentTick
            < pdMS_TO_TICKS(seconds * TIME_SIZE))
    {
#if PRINT_WORKLOAD
        if (i % 10000 * TIME_SIZE == 0)
        {
            printString(str);
        }
        i++;
#endif
    }
}

// Compares the time the task is executed against the time of its deadline
void deadlineMiss(TickType_t xLastWakeTime, TickType_t period, char* pcTask)
{
    TickType_t xPeriodLimit = xLastWakeTime + period;
    TickType_t xActualTime = xTaskGetTickCount();

    //Checks deadline. If task missed it shows information otherwise delays until next period
    if (xPeriodLimit < xActualTime)
    {
        unsigned int ms_miss = (xActualTime - xPeriodLimit) * portTICK_PERIOD_MS;
        char str[50];
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

/*
 * High priority task
 * Workload: 4 time units (2 are inside the critical section)
 * Works 1 time unit->Takes Semaphore-> Works 2 time units-> Gives Semaphore -> Works 1 time units-> Checks if deadline was missed
 * Starts after an initial delay of 3 time units
 */
void vTaskA(void* pvParameters)
{
    TickType_t xLastWakeTime;
    vTaskDelay(pdMS_TO_TICKS(3 * TIME_SIZE));
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();

        printString("Task A: Started\n");
        idleWork(1, "A");
        // Enters cs
        xSemaphoreTake(bin_sem, portMAX_DELAY);
        printString("Task A: sem TAKE\n");
        int i;
        for (i = 0; i < 2; i++)
        {
            idleWork(1, "A");
        }
        printString("Task A: sem GIVE\n");
        xSemaphoreGive(bin_sem);
        // Leaves cs
        idleWork(1, "A");
        printString("Task A: Finished\n");

        deadlineMiss(xLastWakeTime, task_A_period, "A");
    }
}

/*
 * Medium priority task
 * Workload: 4 time units
 * Works 1 time unit->Takes Semaphore-> Works 2 time units-> Gives Semaphore -> Works 1 time units-> Checks if deadline was missed
 * Starts after an initial delay of 6 time units
 */
void vTaskB(void* pvParameters)
{
    TickType_t xLastWakeTime;
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

/*
 * Low priority task
 * Workload: 4 time units
 * Works 1 time unit->Takes Semaphore-> Works 6 time units-> Gives Semaphore -> Works 1 time units-> Checks if deadline was missed
 * Starts after an initial delay of 0 time units
 */
void vTaskC(void* pvParameters)
{
    TickType_t xLastWakeTime;
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();

        printString("Task C: Started\n");
        idleWork(1, "C");
        // Enters cs
        xSemaphoreTake(bin_sem, portMAX_DELAY);
        printString("Task C: sem TAKE\n");
        int i;
        for (i = 0; i < 6; i++)
        {
            idleWork(1, "C");
        }
        printString("Task C: sem GIVE\n");
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
    printString(msg);
    vScheduling();

    while (1)
    {
    }
}
