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
#include "drivers/buttons.h"
#include "drivers/pinout.h"

static volatile unsigned int left_pressed = 0;
static volatile unsigned int right_pressed = 0;

void printString(char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, 13);
}

void configureButtons(void)
{
    ButtonsInit();
}

void configureLEDS(void)
{
    PinoutSet(false, false);

    GPIOPinTypeGPIOOutput(CLP_D1_PORT, CLP_D1_PIN);
    GPIOPinTypeGPIOOutput(CLP_D2_PORT, CLP_D2_PIN);
    GPIOPinTypeGPIOOutput(CLP_D3_PORT, CLP_D3_PIN);
    GPIOPinTypeGPIOOutput(CLP_D4_PORT, CLP_D4_PIN);
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
    unsigned int val = 1;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (left_pressed == 0)
            LEDWrite(CLP_D1, val);
        val = val == 1 ? 0 : 1;
    }
}

void vTaskB(void* pvParameters)
{
    unsigned int val = 2;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (right_pressed == 0)
            LEDWrite(CLP_D2, val);
        val = val == 2 ? 0 : 2;
    }
}

void vTaskC(void* pvParameters)
{
    unsigned int val = 4;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
        LEDWrite(CLP_D3, val);
        val = val == 4 ? 0 : 4;
    }
}

void vTaskD(void* pvParameters)
{
    unsigned int val = 8;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4000));
        LEDWrite(CLP_D4, val);
        val = val == 8 ? 0 : 8;
    }
}

void vTaskE(void* pvParameters)
{
    unsigned char ucDelta, ucState;
    while (1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        if ((GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x1) == 0)
        {
            left_pressed = 1;
            LEDWrite(CLP_D1, 1);
            vTaskDelay(pdMS_TO_TICKS(10000));
            LEDWrite(CLP_D1, 0);
            left_pressed = 0;
        }
    }
}

void vTaskF(void* pvParameters)
{
    unsigned char ucDelta, ucState;
    while (1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        if ((GPIOPinRead(BUTTONS_GPIO_BASE, USR_SW1 | USR_SW2) & 0x2) == 0)
        {
            right_pressed = 1;
            LEDWrite(CLP_D2, 2);
            vTaskDelay(pdMS_TO_TICKS(10000));
            LEDWrite(CLP_D2, 0);
            right_pressed = 0;
        }
    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{

    // Create tasks
    static unsigned char ucParameterToPass1, ucParameterToPass2,
            ucParameterToPass3, ucParameterToPass4, ucParameterToPass5,
            ucParameterToPass6;
    xTaskHandle xHandle1, xHandle2, xHandle3, xHandle4, xHandle5, xHandle6;
    xTaskCreate(vTaskA, "TASKA", configMINIMAL_STACK_SIZE, &ucParameterToPass1,
                tskIDLE_PRIORITY + 1, &xHandle1);
    xTaskCreate(vTaskB, "TASKB", configMINIMAL_STACK_SIZE, &ucParameterToPass2,
                tskIDLE_PRIORITY + 1, &xHandle2);
    xTaskCreate(vTaskC, "TASKC", configMINIMAL_STACK_SIZE, &ucParameterToPass3,
                tskIDLE_PRIORITY + 1, &xHandle3);
    xTaskCreate(vTaskD, "TASKD", configMINIMAL_STACK_SIZE, &ucParameterToPass4,
                tskIDLE_PRIORITY + 1, &xHandle4);
    xTaskCreate(vTaskE, "TASKE", configMINIMAL_STACK_SIZE, &ucParameterToPass5,
                tskIDLE_PRIORITY + 1, &xHandle5);
    xTaskCreate(vTaskF, "TASKF", configMINIMAL_STACK_SIZE, &ucParameterToPass6,
                tskIDLE_PRIORITY + 1, &xHandle6);

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
