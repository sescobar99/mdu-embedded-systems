/*
 * Imports
 */

/*

#define NUMBER_OF_INTS 1000

#include "stopwatch.h"

static volatile uint32_t milisecondsCounter = 0;
static volatile uint32_t secondsCounter = 0;

uint32_t periphEnable(uint32_t periphTimer)
{
    SysCtlPeripheralEnable(periphTimer);

    while (!SysCtlPeripheralReady(periphTimer))
    {
    }

    return 0;
}

void ConfigureUART(void)
{
    periphEnable(SYSCTL_PERIPH_UART0);
    periphEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

extern void Timer0AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Update the periodic interrupt counter.
    //
    milisecondsCounter++;

    if (milisecondsCounter == NUMBER_OF_INTS)
    {
        secondsCounter++;
        milisecondsCounter = 0;
    }
}

// Main Function
int main(void)
{
    uint32_t prevSecondsCounter = 0;
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
            | SYSCTL_CFG_VCO_480),
                                               25000000);

    ConfigureUART();

    periphEnable(SYSCTL_PERIPH_TIMER0);
    configTimer(TIMER0_BASE);
    setTimerLoad(TIMER0_BASE, ui32SysClock/1000);

    // TODO: make API
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntRegister(INT_TIMER0A, Timer0AIntHandler);
    IntEnable(INT_TIMER0A);
    secondsCounter = 0;
    milisecondsCounter = 0;
    // end TODO

    enableTimer(TIMER0_BASE);

    while (1)
    {
        if (prevSecondsCounter != secondsCounter)
        {
            UARTprintf("Segundo: %d\n", secondsCounter);
            prevSecondsCounter = secondsCounter;
        }
    }

    return 0;
}
*/

#include "stopwatch.h"

// Main Function
int main(void)
{
    uint32_t prevCounter = 0;

    initializeSW();

    startSW();

    while (1)
    {
        if (prevCounter != secondsCounter)
        {
            UARTprintf("SEGUNDO: %u\n", secondsCounter);
            prevCounter = secondsCounter;
        }
    }

    return 0;
}

