#include <stopwatch.h>

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

void initializeSW()
{
    configureUART();
    configureTimer();
    configureInterruptions();
}

void startSW(uint32_t initialLoad)
{
    startCounting(initialLoad);
}

void stopSW()
{
    abortCounting();
}

void resetSW(uint32_t initialLoad)
{
    abortCounting();
    startCounting(initialLoad);
}

void printSWOut()
{
    UARTprintf("%u : %u : %u \n", (secondsCounter / 3600) % 24,
               (secondsCounter / 60) % 60, secondsCounter % 60);
}
