#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/tm4c129encpdt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

#define NUMBER_OF_MILISECONDS 1000

static volatile uint32_t milisecondsCounter;
volatile uint32_t secondsCounter;

static bool started = false;

void configureTimer ();
void configureInterruptions ();
void startCounting (uint32_t initialLoad);
void abortCounting ();

extern void Timer0AIntHandler(void);



