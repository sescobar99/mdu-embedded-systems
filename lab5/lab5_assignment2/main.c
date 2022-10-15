#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

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

void printString (char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
}

//*******************************************************
// Tasks code
//*******************************************************

// High priority task
void vTaskA(void* pvParameters)
{
    printString("   Task A: Started    \n");
    printString("   Task A: Finished   \n");
    vTaskDelete(NULL);
}
// Medium priority task
void vTaskB(void* pvParameters)
{
    printString("   Task B: Started    \n");
    printString("   Task B: Finished   \n");
    vTaskDelete(NULL);
}
// Low priority task
void vTaskC(void* pvParameters)
{
    printString("   Task C: Started    \n");
    // Waits 200 ms
    vTaskDelay(200 / portTICK_PERIOD_MS);
    // Enters critical section
    //taskENTER_CRITICAL();
    printString("   Task C: sem take    \n");
    // Waits 700 ms
    vTaskDelay(700 / portTICK_PERIOD_MS);
    printString("   Task C: sem give    \n");
    //taskEXIT_CRITICAL();
    // Waits 200 ms
    vTaskDelay(200 / portTICK_PERIOD_MS);
    printString("   Task C: Finished   \n");
    vTaskDelete(NULL);
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    static unsigned char ucParameterToPass1, ucParameterToPass2,
            ucParameterToPass3;
    xTaskHandle xHandle1, xHandle2, xHandle3;    // Create tasks
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
