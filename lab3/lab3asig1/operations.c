#include <operations.h>

/*
 *  name: configureTimer
 *
 *  Configures the system clock setting its frequency, enables
 *  the timer peripheral, configures the timer splitting it and
 *  setting it to periodic and loads the equivalent ticks to a
 *  milisecond (that means that each milisecond an interruption
 *  will be produced).
 */
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

/**
 *  Name: configureInterruptions
 *
 *  Enables the interruption master and sets the timer interruptions.
 *  Then loads the interruption handler function defined bellow and
 *  enable the interruptions of TIMER0A.
 */
void configureInterruptions ()
{
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntRegister(INT_TIMER0A, Timer0AIntHandler);
    IntEnable(INT_TIMER0A);
}

/**
 *  name: startCounting
 *
 *  Enables the counting of the timer after resetting the context of
 *  variables used in the interruption handler. That will just happen
 *  if the timer is not counting yet.
 */
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

/**
 *  name: abortCounting
 *
 *  Disables the counting of the timer. That will just happen if the
 *  timer is counting.
 */
void abortCounting()
{
    // Interruption disabling
    if (started)
    {
        TimerDisable(TIMER0_BASE, TIMER_A);
        started = false;
    }
}

/**
 *  name: Timer0AIntHandler
 *
 *  Handles the interruption each time is produced. It counts until the
 *  variable milisecondsCounter gets to NUMBER_OF_MILISECONDS (defined as
 *  1000), then it will add 1 to secondsCounter and will reset milisecondsCounter.
 *  That way we get a real time seconds count.
 */
extern void Timer0AIntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer A flag

    milisecondsCounter++;

    if (milisecondsCounter == NUMBER_OF_MILISECONDS)
    {
        secondsCounter++;
        milisecondsCounter = 0;
    }
}
