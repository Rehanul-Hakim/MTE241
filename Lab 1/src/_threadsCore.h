#include <stdint.h>
#include <LPC17xx.h>
#ifndef CLEO_thread
#define CLEO_thread

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void); 
//Returns the address of a new PSP with offset of "offset" bytes from MSP. Be careful with pointer arithmetic! It's best to cast to an integer then back if you're not sure
uint32_t* getNewThreadStack(uint32_t offset); 
//Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP(uint32_t* threadStack); 


#endif