#include "osDefs.h"
#include "_kernelCore.h"
#include "stdio.h"
#include <stdint.h>

//the index of the task running
int cleoIndex = 0;
//number of threads existing
int cleoNums = 0;
//number of threads running/playing
int cleoPlaying = 0;

//mutex boolean variable
//checks if resources are already being used in an interrupt
//false if used, true if available
bool mutex;

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void)	
{
	SHPR3 |= 0xFF << 16;
}

//called by the kernel to schedule which threads to run
void osYield()	
{
	//os is now using resources, so mutex is false
	mutex = false;
	//cleoIndex will only be -1 when it's the very first time running, if it's 
	//the first time running storing and setting status does not happen
	if (cleoIndex >= 0) {	//if thread is sleeping, it wont be set to waking immediately
		if (catArray[cleoIndex].status != SLEEPING) {
		catArray[cleoIndex].status = WAKING;	
		}
		// moving the task pointer down to allocate space for 16 registers to be stored by the handler
		catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 16*4);
	}
	//variable to store the original index to be checked
	int originalIndex = cleoIndex;
	//go to the next task in the array, if we are at the end, loop back to the beginning
	cleoIndex = (cleoIndex+1)%(cleoNums);
	//set the new current task to running/playing
	while (catArray[cleoIndex].status != WAKING && originalIndex != cleoIndex){
		cleoIndex = (cleoIndex+1)%(cleoNums);				
	}
	//if all threads are sleeping, then run the osIdleTask function
	if (originalIndex == cleoIndex && catArray[cleoIndex].status == SLEEPING) {
		osIdleTask();
	}
	//if a thread is found that has the status WAKING
	else {
		//setting the status to PLAYING
		catArray[cleoIndex].status = PLAYING;
		//cleo will play for max 7 seconds before being switched
		catArray[cleoIndex].playTime = 50;
		//os is done using resources, so mutex is true
		mutex = true;
		//trigger the PendSV interrupt
		ICSR |= 1 << 28;
		__asm("isb");
	}
}

//initialize anything that the first thread needs and triggers the interrupt to start the first 
//thread, and switches SP to PSP
void kernel_start(void)
{
	//create the idle task
	createThread(osIdleTask);
	//is there a thread to run? if yes:
	if (cleoNums > 0) {
		//telling the yield function that this is the first thread we are creating
		cleoIndex = -1;
		//set to threading mode, and set the stack pointer to the beginning of the first thread we are creating
		setThreadingWithPSP(catArray[0].taskPointer);
		//run the context switching so the first thread starts running
		osYield();
	}
}

//this performs the task/context switching
int task_switch(void)
{
	//set the new PSP
	__set_PSP((uint32_t)catArray[cleoIndex].taskPointer);
	return 1;
}

//Idle thread that will run when either all threads are sleeping or there are no threads
void osIdleTask()
{
	while (1) {
		printf("In idle.\n");
		osYield();
	}
}
