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
#include "inc/tm4c129encpdt.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"
#include "driverlib/adc.h"

#define NULL_CHARACTER 0
#define BACKSPACE 8
#define LINE_FEED 10
#define CARRIAGE_RETURN 13
#define SPACE 32
#define DELETE 127
#define DELETE_SIZE 30

static volatile char* reverseLineFeed = "\033[A";
static volatile char empty[DELETE_SIZE];

// Buffer
static volatile uint32_t pui32Buffer[6];
// Queues
static volatile QueueHandle_t microphoneQueue, joystickQueue,
        accelerometerQueue;
// mutex
static volatile SemaphoreHandle_t samplingValuesBinarySem;

typedef struct
{
    uint32_t value;
} microphoneMsg;

typedef struct
{
    uint32_t value[2];
} joystickMsg;
typedef struct
{
    uint32_t value[3];
} accelerometerMsg;

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
//    UARTCharPut(UART0_BASE, CARRIAGE_RETURN);
}

// configure ADC
void configureADC(void)
{
    // Enabling peripherals
    SysCtlClockFreqSet(
    SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_CFG_VCO_480,
                       16000000);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {

    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {

    }
    // Configuring pins
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);    // Joystick x
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);    // Joystick y
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);    // Microphone
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);    // Accelerometer x
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);    // Accelerometer y
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);    // Accelerometer z
    // Configure ADC sequencer
    ADCIntEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);    // Clear ADC Interrupt
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    //CH8 Microphone
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
    ADC_CTL_CH8);

    //CH0 Joystick y
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1,
    ADC_CTL_CH0);
    //CH9 Joystick x
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2,
    ADC_CTL_CH9);

    //CH1 CH2 CH3 Accelerometer
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3,
    ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4,
    ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5,
                             (ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH3));

    ADCSequenceEnable(ADC0_BASE, 0);
}

//*******************************************************
// Tasks code
//*******************************************************
void vADCSampler(void* pvParameters)
{
    while (1)
    {
        ADCProcessorTrigger(ADC0_BASE, 0);
        while (!ADCIntStatus(ADC0_BASE, 0, false))
        {
        }
        ADCIntClear(ADC0_BASE, 0);
        xSemaphoreTake(samplingValuesBinarySem, portMAX_DELAY);
        ADCSequenceDataGet(ADC0_BASE, 0, pui32Buffer);
        xSemaphoreGive(samplingValuesBinarySem);
    }
}

void vMicrophoneManager(void* pvParameters)
{
    uint32_t value;
    double voltage, lastVoltage, db;
    vTaskDelay(pdMS_TO_TICKS(4));
    while (1)
    {
        xSemaphoreTake(samplingValuesBinarySem, portMAX_DELAY);
        value = pui32Buffer[0];
        xSemaphoreGive(samplingValuesBinarySem);

        // Process the value
        value = abs(2024 - value);
        voltage = (5.0f * value) / 2024.0f;
        db = 20 * log(voltage / lastVoltage);
        lastVoltage = voltage;

        // Send msg to queue
        microphoneMsg msg;
        msg.value = db;
        xQueueSendToBack(microphoneQueue, (void* ) &msg, portMAX_DELAY);

        // Waits 5 ms
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void vJoystickManager(void* pvParameters)
{
    uint32_t value[2];
    vTaskDelay(pdMS_TO_TICKS(5));
    while (1)
    {
        xSemaphoreTake(samplingValuesBinarySem, portMAX_DELAY);
        value[0] = pui32Buffer[1];
        value[1] = pui32Buffer[2];
        xSemaphoreGive(samplingValuesBinarySem);

        // Send msg to queue
        joystickMsg msg;
        msg.value[0] = value[0];
        msg.value[1] = value[1];
        xQueueSendToBack(joystickQueue, (void* ) &msg, portMAX_DELAY);

        // Waits 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void vAccelerometerManager(void* pvParameters)
{
    uint32_t value[3];
    vTaskDelay(pdMS_TO_TICKS(15));
    while (1)
    {
        xSemaphoreTake(samplingValuesBinarySem, portMAX_DELAY);
        value[0] = pui32Buffer[3];
        value[1] = pui32Buffer[4];
        value[2] = pui32Buffer[5];
        xSemaphoreGive(samplingValuesBinarySem);

        // Send msg to queue
        accelerometerMsg msg;
        msg.value[0] = value[0];
        msg.value[1] = value[1];
        msg.value[2] = value[2];
        xQueueSendToBack(accelerometerQueue, (void* ) &msg, portMAX_DELAY);

        // Waits 20 ms
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
void vGatekeeper(void* pvParameters)
{
    uint32_t averageMicrophone;
    uint32_t averageJoystick[2];
    uint32_t averageJoystick0;
    uint32_t averageAccelerometer[3];
    uint32_t counter;
    microphoneMsg mmsg;
    joystickMsg jmsg;
    accelerometerMsg amsg;
    char str[10];
    vTaskDelay(pdMS_TO_TICKS(400));

    printString("Microphone: 0db\r\n");
    printString("Joystick: 0, 0\r\n");
    printString("Accelerometer: 0, 0, 0\r\n");
    while (1)
    {
        printString(reverseLineFeed);
        printString(reverseLineFeed);
        printString(reverseLineFeed);
        printString("Microphone:                        \r\n");
        printString("Joystick:                          \r\n");
        printString("Accelerometer:                     \r\n");
//        printString("\r");

        averageMicrophone = 0;
        averageJoystick[0] = 0;
        averageJoystick[1] = 0;
        averageAccelerometer[0] = 0;
        averageAccelerometer[1] = 0;
        averageAccelerometer[2] = 0;
        counter = 0;

        while (xQueueReceive(microphoneQueue, &mmsg, (TickType_t) 0) == pdPASS)
        {
            averageMicrophone += mmsg.value;
            counter++;
        }
        averageMicrophone /= counter;
        counter = 0;

        while (xQueueReceive(joystickQueue, &jmsg, (TickType_t) 0) == pdPASS)
        {
            averageJoystick[0] += jmsg.value[0];
            averageJoystick[1] += jmsg.value[1];
            counter++;
        }
        averageJoystick[0] /= counter;
        averageJoystick[1] /= counter;
        averageJoystick0 = averageJoystick[0];
        counter = 0;

        while (xQueueReceive(accelerometerQueue, &amsg, (TickType_t ) 0)
                == pdPASS)
        {
            averageAccelerometer[0] += amsg.value[0];
            averageAccelerometer[1] += amsg.value[1];
            averageAccelerometer[2] += amsg.value[2];
            counter++;
        }
        averageAccelerometer[0] /= counter;
        averageAccelerometer[1] /= counter;
        averageAccelerometer[2] /= counter;
        counter = 0;

        printString(reverseLineFeed);
        printString(reverseLineFeed);
        printString(reverseLineFeed);
        printString("Microphone: ");
        itos(averageMicrophone, str);
        printString(str);
        printString("\n\r");
        // print
        printString("Joystick: ");
        itos(averageJoystick[1], str);
        printString(str);
        printString(" , ");
        itos(averageJoystick0, str);
        printString(str);
        printString("\n\r");

        printString("Accelerometer: ");
        itos(averageAccelerometer[0], str);
        printString(str);
        printString(", ");
        itos(averageAccelerometer[1], str);
        printString(str);
        printString(",");
        itos(averageAccelerometer[2], str);
        printString(str);
        printString("\r");

//        printString(reverseLineFeed);
//        printString(empty);
//        printString(empty);

        averageJoystick0 = 0;
        vTaskDelay(pdMS_TO_TICKS(40));

    }
}

//*******************************************************
// Scheduling
//*******************************************************

void vScheduling()
{
    // Create tasks
    xTaskCreate(vADCSampler, "ADCSAMPLER", configMINIMAL_STACK_SIZE, (void* ) 4,
                tskIDLE_PRIORITY + 1, NULL);
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
    int i = 0;
    for (i = 0; i < DELETE_SIZE; i++)
    {
        empty[i] = SPACE;
    }
    empty[DELETE_SIZE - 1] = NULL_CHARACTER;
    microphoneQueue = xQueueCreate((unsigned portBASE_TYPE) 8,
                                   (unsigned portBASE_TYPE) (2 * sizeof(int)));
    joystickQueue = xQueueCreate((unsigned portBASE_TYPE) 4,
                                 (unsigned portBASE_TYPE) (3 * sizeof(int)));
    accelerometerQueue = xQueueCreate(
            (unsigned portBASE_TYPE) 2,
            (unsigned portBASE_TYPE) (4 * sizeof(int)));

    samplingValuesBinarySem = xSemaphoreCreateBinary(); // Create semaphore

    configureUART(); // Init UART
    configureADC(); // Init ADC

    xSemaphoreGive(samplingValuesBinarySem);
    vScheduling(); // Start scheduler
}
