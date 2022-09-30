#include "stopwatch.h"

void menu()
{
    UARTprintf("STOPWATCH \n");
    UARTprintf("To manage the stopwatch there is the next controls: \n");
    UARTprintf(" - To start press 1 \n");
    UARTprintf(" - To stop once is running press 2 \n");
    UARTprintf(" - To reset once is running press 3 \n");
}

static void configureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

// Main Function
int main(void)
{
    uint32_t prevCounter = 0;
    uint32_t valueLen = 3;
    int32_t initialLoad = -1;
    char buffer;
    char input;

    configureUART();

    initializeSW();

    menu();

    while (1)
    {
        if (UARTCharsAvail(UART0_BASE))
        {
            input = UARTCharGetNonBlocking(UART0_BASE);
        }

        if (prevCounter != secondsCounter)
        {
            printSWOut();
            prevCounter = secondsCounter;
        }

        if (input == '1')
        {
            initialLoad = 0;

            UARTprintf("\nIntroduce the initial hour: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (((uint32_t) atoi(&buffer)) * 3600);
            UARTprintf("\nIntroduce the initial minute: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (((uint32_t) atoi(&buffer)) * 60);
            UARTprintf("\nIntroduce the initial second: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (uint32_t) atoi(&buffer);

            startSW((uint32_t) initialLoad);
        }
        if (input == '2')
        {
            stopSW();
            initialLoad = -1;
            menu();
        }
        if (input == '3')
            if (initialLoad > -1)
                resetSW((uint32_t) initialLoad);

        input = 0;
    }

    return 0;
}

