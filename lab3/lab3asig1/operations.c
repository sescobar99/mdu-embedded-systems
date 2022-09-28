#include <operations.h>

void configureTimer()
{
    uint32_t ui32SysClock = SysCtlClockFreqSet(
            (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL
                    | SYSCTL_CFG_VCO_480),
            25000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC));
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32SysClock / 1000);
}

void configureInterruptions ()
{
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntRegister(INT_TIMER0A, Timer0AIntHandler);
    IntEnable(INT_TIMER0A);
}

void startCounting(uint32_t initialLoad)
{
    // Interruption configuration
    if (!started)
    {
        milisecondsCounter = 0;
        secondsCounter = initialLoad;
        started = true;

        TimerEnable(TIMER0_BASE, TIMER_A);
    }
}

void abortCounting()
{
    // Interruption disabling
    if (started)
    {
        TimerDisable(TIMER0_BASE, TIMER_A);
        started = false;
    }
}

// Interrrupt handler
extern void Timer0AIntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer A flag

    milisecondsCounter++;   // Update the periodic interrupt counter.

    if (milisecondsCounter == NUMBER_OF_MILISECONDS)
    {
        secondsCounter++;
        milisecondsCounter = 0;
    }
}
