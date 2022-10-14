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
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"

void configureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
       GPIOPinConfigure(GPIO_PA0_U0RX);
       GPIOPinConfigure(GPIO_PA1_U0TX);
       GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
       UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
       UARTStdioConfig(0, 115200, 16000000);

}

//*******************************************************
// Tasks code
//*******************************************************

// High priority task
void vTask1(void* pvParameters)
{
    while (1)
    {
        UARTCharPut(UART0_BASE, '1');
    }
}
// Medium priority task
void vTask2(void* pvParameters)
{
    while (1)
    {
        UARTCharPut(UART0_BASE, '2');
    }
}
// Low priority task
void vTask3(void* pvParameters)
{
    while (1)
    {
        UARTCharPut(UART0_BASE, '3');
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    static unsigned char ucParameterToPass;
    xTaskHandle xHandle1, xHandle2, xHandle3;    // Create tasks
    xTaskCreate(vTask1, "TASK1", configMINIMAL_STACK_SIZE, &ucParameterToPass,
                tskIDLE_PRIORITY + 3, &xHandle1);
    xTaskCreate(vTask2, "TASK2", configMINIMAL_STACK_SIZE, &ucParameterToPass,
                tskIDLE_PRIORITY + 2, &xHandle2);
    xTaskCreate(vTask3, "TASK3", configMINIMAL_STACK_SIZE, &ucParameterToPass,
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
    UARTCharPut(UART0_BASE, 'c');
    //vScheduling();

    while (1);
}
