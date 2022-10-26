#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"
#ifndef CLEO_thread
#define CLEO_thread

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void); 

//Returns the address of a new PSP with offset of "offset" bytes from MSP.
uint32_t* getNewThreadStack(uint32_t offset); 

//Sets up the threads
int createThread (void (*task)(void* args));

//Using SysTick to context switch
void SysTick_Handler(void);

#endif
