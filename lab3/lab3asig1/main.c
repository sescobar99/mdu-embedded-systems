#include "stopwatch.h"

/**
 *  Name: menu
 *
 *  Uses the methods available in UART library to print the
 *  needed menu to show the users how to control the program
 *  by serial inputs.
 */
void menu()
{
    UARTprintf("STOPWATCH \n");
    UARTprintf("To manage the stopwatch there is the next controls: \n");
    UARTprintf(" - To start press 1 \n");
    UARTprintf(" - To stop once is running press 2 \n");
    UARTprintf(" - To reset once is running press 3 \n");
}

/**
 *  Name: configureUART
 *
 *  Configures the peripherals, the general purpose input/output
 *  and the UART settings.
 */
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

/**
 *  Name: main
 */
int main(void)
{
    /*
     *  Declares local variables
     */
    uint32_t prevCounter = 0;
    uint32_t valueLen = 3;
    int32_t initialLoad = -1;
    char buffer;
    char input;

    configureUART();

    initializeSW(); // Initialize the stop watch

    menu();

    // Running loop
    while (1)
    {
        /*
         *  Gets a serial input while the program stills running
         */
        if (UARTCharsAvail(UART0_BASE))
        {
            input = UARTCharGetNonBlocking(UART0_BASE);
        }

        /*
         *  Checks if there is the need to update the counter
         */
        if (prevCounter != secondsCounter)
        {
            printSWOut();
            prevCounter = secondsCounter;
        }

        /*
         *  Depending on the received input it does:
         *  Input = 1:
         *      Asks for the initial value of the counter,
         *      the user must introduce hours, minutes, seconds.
         *      Then starts the counter.
         */
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
        /*
         *  Input = 2:
         *      Stops the stop watch and shows the menu again.
         */
        if (input == '2')
        {
            stopSW();
            initialLoad = -1;
            menu();
        }
        /*
         *  Input = 3:
         *      Check if the initial load is less than 0 (if is
         *      true the counter didn't start yet) and resets the
         *      stop watch.
         */
        if (input == '3')
            if (initialLoad > -1)
                resetSW((uint32_t) initialLoad);

        input = 0;
    }

    return 0;
}

