#include "osDefs.h"
#include "_kernelCore.h"
#include <stdint.h>

void kernelInit(void)	//initializes memory structures and interrupts necessary to run the kernel
{
	SHPR3 |= 0xFF << 16;
}
void osSched(void)	//called by the kernel to schedule which threads to run
{
	ICSR |= 1 << 28;
	__asm("isb");
}

//initialize anything that the first thread needs before it gets going, and switches SP to PSP
void kernel_start(void)
{
	//simplest way
	void osSched();
}
int task_switch(void)
{
	//set the new PSP
	__set_PSP((uint32_t)osThreads[osCurrentTask].taskStack);
	return 1;	//You are free to use this return value in your
						//assembly eventually. It will be placed in r0, so be sure to
						//access it before overwriting r0.
}
