#include <LPC17xx.h>
#include "stdio.h"
#include "_threadsCore.h"

//array of structs to store threads
cleoThread catArray[maxThreads]; 

uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0;
	return (uint32_t*) *ptrmsp;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	if (offset > threadStackPool)	//Max size of stack is defined as 2000
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
  __set_CONTROL(0x02);	//Set to threading mode
	__set_PSP(pspval);

}

//creating a new thread, returns index of new thread if successful, returns -1 if array is full
int createThread (void (*task)(void* args))
{
	if (cleoNums < maxThreads)
	{
		catArray[cleoNums].threadFunc = task;
		//before the thread is created, the status is set to waiting/sleeping
		catArray[cleoNums].status = SLEEPING;
		//getting stack pointer to the beginning of thread, under the stack reserved for handler mode and existing threads
		uint32_t* sp = getNewThreadStack((cleoNums + 1)*threadStackSize);

		catArray[cleoNums].taskPointer = sp;
		
		//shift by 24 so that it is set to threading mode
		*--sp = 1<<24;
		
		//task a program counter, set to the current running function
		*--sp = (uint32_t)task;	//PC
		
		//sets contents of registers
		*--sp = 0xE; //LR
		*--sp =	0xC; //R12
		*--sp =	0x3; //R3
		*--sp = 0x2; //R2
		*--sp = 0x1; //R1
		*--sp = 0x0; //R0
		
		//sets contents of R11 to R4
		*--sp = 0xB; //R11
		*--sp = 0xA; //R10
		*--sp = 0x9; //R9
		*--sp = 0x8; //R8
		*--sp = 0x7; //R7
		*--sp = 0x6; //R6
		*--sp = 0x5; //R5
		*--sp = 0x4; //R4
		
		//the new thread in the array 
		cleoNums++;
		cleoPlaying++;
		return cleoNums-1;
	} 
	return -1;
}
