#include <LPC17xx.h>
#include "stdio.h"
#include "osDefs.h"
#include "_threadsCore.h"

uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0;
	return (uint32_t*) *ptrmsp;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	if (offset > 2000)	//Max size of stack is defined as 2000
	{
		return 0;
	}
	unsigned int mspval = (unsigned int)getMSPInitialLocation();
	unsigned int pspval = mspval - offset;
	if (pspval%8!=0)	//If not div by 8, offsets stack by an additional sizeof(uint32_t)
	{
		pspval = pspval - sizeof(uint32_t);
	}
	return (unsigned int*)pspval;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	uint32_t pspval = (uint32_t) threadStack;
	__set_PSP(pspval);
	__set_CONTROL(0x02);	//Set to threading mode
}

void threadFunction (void* args){
	while(1)
	{
		printf("This is thread 1.");
		osYield();
	}
}
void osYield(void)
{
	uint32_t* sp = getNewThreadStack(0);
	*--sp = 1<<24;
	*--sp = (uint32_t)threadFunction;	//threadFunction is a function pointer
	
	//sets contents of registers LR, R12, R3, R2, R1, R0
	*--sp = 0xF;
	*--sp =	0xE;
	*--sp =	0xD;
	*--sp = 0xC;
	*--sp = 0xB;
	*--sp = 0xA;
	//sets contents of R11 throught R4
	*--sp = 0x9;
	*--sp = 0x8;
	*--sp = 0x7;
	*--sp = 0x6;
	*--sp = 0x5;
	*--sp = 0x4;
	*--sp = 0x3;
}

