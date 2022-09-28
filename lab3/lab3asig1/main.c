#include "stopwatch.h"

void menu()
{
    UARTprintf("STOPWATCH \n");
    UARTprintf("To manage the stopwatch there is the next controls: \n");
    UARTprintf(" - To start press 1 \n");
    UARTprintf(" - To stop once is running press 2 \n");
    UARTprintf(" - To reset once is running press 3 \n");
}

// Main Function
int main(void)
{
    uint32_t prevCounter = 0;
    uint32_t valueLen = 3;
    uint32_t initialLoad = 0;
    char buffer;
    char input;

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
            UARTprintf("\nIntroduce the initial hour: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (((uint32_t) atoi(&buffer)) * 3600);
            UARTprintf("\nIntroduce the initial minute: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (((uint32_t) atoi(&buffer)) * 60);
            UARTprintf("\nIntroduce the initial second: \n");
            UARTgets(&buffer, valueLen);
            initialLoad += (uint32_t) atoi(&buffer);

            startSW(initialLoad);
        }
        if (input == '2')
        {
            stopSW();
            initialLoad = 0;
            menu();
        }
        if (input == '3')
            if (initialLoad)
                resetSW(initialLoad);

        input = 0;
    }

    return 0;
}

