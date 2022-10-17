#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#define SECOND 1000000

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

void configureButtons(void)
{
    ButtonsInit();
}

void configureLEDS(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
}

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

//*******************************************************
// Tasks code
//*******************************************************

void vTaskA(void* pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskB(void* pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vTaskC(void* pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void vTaskD(void* pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create semaphore
    bin_sem = xSemaphoreCreateBinary();

    // Create tasks
    static unsigned char ucParameterToPass1, ucParameterToPass2,
            ucParameterToPass3, ucParameterToPass4;
    xTaskHandle xHandle1, xHandle2, xHandle3, xHandle4;
    xTaskCreate(vTaskA, "TASKA", configMINIMAL_STACK_SIZE, &ucParameterToPass1,
                tskIDLE_PRIORITY + 1, &xHandle1);
    xTaskCreate(vTaskB, "TASKB", configMINIMAL_STACK_SIZE, &ucParameterToPass2,
                tskIDLE_PRIORITY + 1, &xHandle2);
    xTaskCreate(vTaskC, "TASKC", configMINIMAL_STACK_SIZE, &ucParameterToPass3,
                tskIDLE_PRIORITY + 1, &xHandle3);
    xTaskCreate(vTaskC, "TASKD", configMINIMAL_STACK_SIZE, &ucParameterToPass4,
                tskIDLE_PRIORITY + 1, &xHandle4);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{
    configureUART();
    configureLEDS();
    configureButtons();
    vScheduling();

    while (1)
    {
    }
}
