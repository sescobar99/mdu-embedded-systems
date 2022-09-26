#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"

uint32_t periphEnable(uint32_t periphTimer);
uint32_t configTimer(uint32_t timerBaseAddress);
uint32_t enableTimer(uint32_t timerBaseAddress);
uint32_t disableTimer(uint32_t timerBaseAddress);
uint64_t getTimerValue(uint32_t timerBaseAddress);
uint32_t setTimerValue(uint32_t timerBaseAddress, uint32_t timerLoad);
