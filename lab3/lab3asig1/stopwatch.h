#include <operations.h>

/**
 * Interface to control the Stop Watch
 */
static void ConfigureUART(void);
void initializeSW();
void startSW(uint32_t initialLoad);
void stopSW();
void resetSW(uint32_t initialLoad);
void printSWOut();
