/*
 * Imports
 */
#define EXT_CLOCK_F 32768
#define NUMBER_OF_INTS 60

#include "timerManager.h"

static volatile uint32_t g_ui32Counter = 0;

void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
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
    g_ui32Counter++;

    //
    // Once NUMBER_OF_INTS interrupts have been received, turn off the
    // TIMER0B interrupt.
    //
    if (g_ui32Counter == NUMBER_OF_INTS)
    {
        //
        // Disable the Timer0B interrupt.
        //
        IntDisable(INT_TIMER0A);

        //
        // Turn off Timer0B interrupt.
        //
        TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        //
        // Clear any pending interrupt flag.
        //
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }
}

// Main Function
int main(void)
{
    uint32_t ui32PrevCount = 0;
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
            | SYSCTL_CFG_VCO_480),
    16000);

    ConfigureUART();

    periphEnable(SYSCTL_PERIPH_TIMER0);
    configTimer(TIMER0_BASE);
    setTimerValue(TIMER0_BASE, ui32SysClock);

    // TODO: make API
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntRegister(INT_TIMER0A, Timer0AIntHandler);
    g_ui32Counter = 0;
    IntEnable(INT_TIMER0A);
    // end TODO

    enableTimer(TIMER0_BASE);

    while (1)
    {
        if (g_ui32Counter != ui32PrevCount)
        {
            UARTprintf("Segundo: %d\n", g_ui32Counter);
            ui32PrevCount = g_ui32Counter;
        }
    }

    return 0;
}
