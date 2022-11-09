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
	SHPR3 |= 0xFF << 16; //set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFFU << 24; //Set the priority of SysTick to be the weakest
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC the be the strongest
}

//called by the kernel to schedule which threads to run
void osYield()	
{
	//Trigger the SVC right away.
	__ASM("SVC #0");
}

//cleoScheduler determines which task to run next by checking all deadlines
void cleoScheduler()
{
	int i;
	int closestToDinner = 0; //stores index of thread with closest deadline
	bool foundCat = false; //if there is a potential "WAKING" thread to run next
	//iterate through all the threads and check their deadlines
	for (i = 0; i < cleoNums-1; ++i){
		//check if there is a thread with a closer deadline approaching that must run first
		//and that thread cannot be sleeping
		if (catArray[i].dinnerTime < catArray[closestToDinner].dinnerTime && catArray[i].status != SLEEPING)
		{
			closestToDinner = i;
			foundCat = true; //there has been a thread found to run next
		}
	}
	if (foundCat == false) //if all threads are sleeping
	{
		//go to the idle task, which is always at the end
		cleoIndex = cleoNums - 1;
		//run the idle task
		catArray[cleoIndex].status = PLAYING;
	}
	else 
	{
		//the current index will go to the thread with closest deadline
		cleoIndex = closestToDinner;
		//run closest deadline thread
		catArray[cleoIndex].status = PLAYING;
		//set the playtime for this thread
		catArray[cleoIndex].playTime = cleoPlayTime;
	}
}

//SVC_Handler_Main gets the value of the system call's immediate
void SVC_Handler_Main(uint32_t *svc_args)
{
	char call = ((char*)svc_args[6])[-2];
	if (call == YIELD_SWITCH) 
	{
		//cleoIndex will only be -1 when it's the very first time running, if it's 
		//the first time running storing and setting status does not happen
		if (cleoIndex >= 0) 
			{	
				//if thread is sleeping, it won't be set to waking immediately
				if (catArray[cleoIndex].status != SLEEPING)
				{
					catArray[cleoIndex].status = WAKING;	
				}
				// moving the task pointer down to allocate space for 16 registers to be 
				//stored by the handler
				catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 8*4);
			}
			//run the scheduler
			cleoScheduler();
			//trigger the PendSV interrupt
			ICSR |= 1 << 28;
			__asm("isb");
	}
}

//initialize anything that the first thread needs and triggers the interrupt to start the first 
//thread, and switches SP to PSP
void kernel_start(void)
{
	//create the idle task as the last thread in the array
	createThread(osIdleTask, 349580439); //THIS IS TEMP VALUE LOL MUST FIX!!!!!!!!!!!
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
		osYield();
	}
}
