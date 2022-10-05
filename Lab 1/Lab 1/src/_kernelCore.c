#include "osDefs.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include <stdint.h>

//get the index of the task running
extern int cleoIndex;

void kernelInit(void)	//initializes memory structures and interrupts necessary to run the kernel
{
	SHPR3 |= 0xFF << 16;
}
void osYield(void)	//called by the kernel to schedule which threads to run
{
	// determine if the switch is necessary: if not only one task running, determine which thread to switch to
	if (cleoNums > 1) {
		// save a useful offset of the current thread's stack pointer somewhere so it can be accessed again
		catArray[cleoIndex].status = SLEEPING;	
		// moving the task pointer down to allocate space for 16 registers to be stored by the handler
		catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 16*4);
		//go to the next task in the array, if we are at the end, loop back to the beginning
		cleoIndex = (cleoIndex+1)%(cleoNums);
		//set the new current task to running/playing
		catArray[cleoIndex].status = PLAYING;
		// trigger the PendSV interrupt
		ICSR |= 1 << 28;
		__asm("isb");
	}
}

//initialize anything that the first thread needs before it gets going, and switches SP to PSP
void kernel_start(void)
{
	if (cleoNums > 0) {
		//set to threading mode
		setThreadingWithPSP(catArray[0].taskPointer);
		cleoIndex = 0;
		//call the first task in the array, which will call yield on its own and round robin through the other tasks
		catArray[0].threadFunc(0);
	}
	// makes sure that the function does not end
	while (1);
}
int task_switch(void)
{
	//set the new PSP
	__set_PSP((uint32_t)catArray[cleoIndex].taskPointer);
	return 1;	//You are free to use this return value in your
						//assembly eventually. It will be placed in r0, so be sure to
						//access it before overwriting r0.
}
