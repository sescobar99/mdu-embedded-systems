#include <stopwatch.h>

void initializeSW()
{
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
