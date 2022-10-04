#include <stopwatch.h>

/**
 *  name: initializeSW
 *
 *  Calls the configureTimer and configureInterruptions functions
 *  to configure those things.
 */
void initializeSW()
{
    configureTimer();
    configureInterruptions();
}

/**
 *  name: startSW
 *
 *  Starts the counting.
 */
void startSW(uint32_t initialLoad)
{
    startCounting(initialLoad);
}

/**
 *  name: stopSW
 *
 *  Stop the counting.
 */
void stopSW()
{
    abortCounting();
}

/**
 *  name: resetSW
 *
 *  Stop the counting and then it starts it again.
 */
void resetSW(uint32_t initialLoad)
{
    abortCounting();
    startCounting(initialLoad);
}

/**
 * name: printSWOut
 *
 * Prints the formated counter using UART library function.
 */
void printSWOut()
{
    UARTprintf("%u : %u : %u \n", (secondsCounter / 3600) % 24,
               (secondsCounter / 60) % 60, secondsCounter % 60);
}
