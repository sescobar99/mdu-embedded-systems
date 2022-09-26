#include "timerManager.h"

uint32_t periphEnable(uint32_t periphTimer)
{
    SysCtlPeripheralEnable(periphTimer);

    while (!SysCtlPeripheralReady(periphTimer))
    {
    }

    return 0;
}

uint32_t configTimer(uint32_t timerBaseAddress)
{
    TimerConfigure(timerBaseAddress, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC));

    return 0;
}

uint32_t enableTimer(uint32_t timerBaseAddress)
{
    TimerEnable(timerBaseAddress, TIMER_A);

    return 0;
}

uint32_t disableTimer(uint32_t timerBaseAddress)
{
    TimerDisable(timerBaseAddress, TIMER_A);

    return 0;
}

uint64_t getTimerValue(uint32_t timerBaseAddress)
{
    return TimerValueGet(timerBaseAddress, TIMER_A);
}

uint32_t setTimerValue(uint32_t timerBaseAddress, uint32_t timerLoad)
{
    TimerLoadSet(timerBaseAddress, TIMER_A, timerLoad);

    return 0;
}


