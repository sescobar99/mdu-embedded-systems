#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

// configure ADC
void configureADC(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {

    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

}

// configure UART
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

// int to string
void itos(int number, char* str)
{
    const int size = 10;
    int digits[size];
    int i = 0, e = 0;
    while (1)
    {
        digits[i++] = number % 10;
        number /= 10;
        if (number == 0)
            break;
    }
    i -= 1;
    while (i > -1)
    {
        str[e++] = digits[i--] + '0';
    }
    str[e] = 0;
}

// using UART to print strings
void printString(char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *(str++));
    }
    UARTCharPut(UART0_BASE, CARRIAGE_RETURN);
}

//*******************************************************
// Tasks code
//*******************************************************
void vMicrophoneManager(void* pvParameters)
{
    while (1)
    {
    }
}
void vJoystickManager(void* pvParameters)
{
    while (1)
    {
    }
}
void vAccelerometerManager(void* pvParameters)
{
    while (1)
    {
    }
}
void vGatekeeper(void* pvParameters)
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
    xTaskCreate(vGatekeeper, "GATEKEEPER", configMINIMAL_STACK_SIZE, (void* ) 0,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vMicrophoneManager, "MICROPHONE", configMINIMAL_STACK_SIZE,
                (void* ) 1, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vJoystickManager, "JOYSTICK", configMINIMAL_STACK_SIZE,
                (void* ) 2, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vAccelerometerManager, "ACCELEROMETER",
                configMINIMAL_STACK_SIZE, (void* ) 3, tskIDLE_PRIORITY + 1,
                NULL);

    // Start scheduler
    vTaskStartScheduler();
}

//*******************************************************
// Main
//*******************************************************
int main(void)
{

    configureADC();
    configureUART(); // Init UART
    vScheduling(); // Start scheduler

}