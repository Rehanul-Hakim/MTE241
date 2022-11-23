#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"
#ifndef CLEO_thread
#define CLEO_thread

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void); 

//Returns the address of a new PSP with offset of "offset" bytes from MSP.
uint32_t* getNewThreadStack(uint32_t offset); 

//Using SysTick for time dependant operations and preemptive switching
void SysTick_Handler(void);

//Function that turns the thread's state to sleep after running
void cleoSleep(int userSleepTime);

#endif