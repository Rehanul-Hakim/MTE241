#include <LPC17xx.h>
#include "stdio.h"
#include "_threadsCore.h"

//Array of structs to store threads.
//catArray is an array of size maxThreads containing cleoThread
cleoThread catArray[maxThreads]; 

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0;
	return (uint32_t*) *ptrmsp;
}

//Returns the address of a new PSP with offset of "offset" bytes from MSP.
uint32_t* getNewThreadStack(uint32_t offset)
{
	if (offset > threadStackPool)	//Check if offset is greater than the size of stack pool (2000)
	{
		return 0;	//return error
	}
	unsigned int mspval = (unsigned int)getMSPInitialLocation();
	unsigned int pspval = mspval - offset;
	if (pspval%8!=0)	//If not div by 8, offsets stack by an additional sizeof(uint32_t)
	{
		pspval = pspval - sizeof(uint32_t);
	}
	return (unsigned int*)pspval;
}

//Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP(uint32_t* threadStack)
{
	uint32_t pspval = (uint32_t) threadStack;
	__set_PSP(pspval);
	__set_CONTROL(0x02);	//Set to threading mode
}

//creating a new thread, returns index of new thread if successful, returns -1 if array is full
int createThread (void (*task)(void* args))
{
	if (cleoNums < maxThreads)
	{
		catArray[cleoNums].threadFunc = task;
		//before the thread is created, the status is set to waking
		catArray[cleoNums].status = WAKING;
		//getting stack pointer to the beginning of thread, under the stack reserved for handler mode and existing threads
		uint32_t* sp = getNewThreadStack((cleoNums + 1)*threadStackSize);

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
		
		//assign the taskpointer (stack pointer) to sp
		catArray[cleoNums].taskPointer = sp;
		
		//the new thread in the array 
		cleoNums++;
		cleoPlaying++;
		return cleoNums-1;
	} 
	return -1;
}

//Timer that decrements every ms
void SysTick_Handler(void)
{
	int i;
	for (i = 0; i < cleoNums-1; ++i){
		if (catArray[i].status == SLEEPING && catArray[i].sleepTime > 0) {
			catArray[i].sleepTime = catArray[i].sleepTime - 1;
		}
		else if (catArray[i].status == SLEEPING && catArray[i].sleepTime == 0) {
			catArray[i].status = WAKING;
		}
		else if (catArray[i].status == PLAYING && catArray[i].playTime > 0) {
			catArray[i].playTime = catArray[i].playTime - 1;
		}
		else if (mutex == true && catArray[i].status == PLAYING && catArray[i].playTime == 0) {
			//only initiate context switch if mutex is true
			//i.e resources are available to be used by the os
			mutex = false;
			catArray[i].status = WAKING;	
			// moving the task pointer down to allocate space for 8 registers to be stored by the handler
			catArray[i].taskPointer = (uint32_t*)(__get_PSP() - 8*4);
			//variable to store the original index to be checked
			int originalIndex = i;
			//go to the next task in the array, if we are at the end, loop back to the beginning
			i = (i+1)%(cleoNums);
			//set the new current task to running/playing
			while (catArray[i].status != WAKING && originalIndex != i){
				i = (i+1)%(cleoNums);				
			}
			//if all threads are sleeping, then run the osIdleTask function
			if (originalIndex == i && catArray[i].status == SLEEPING) {
				osIdleTask();
			}
			else {
				//if a thread is found that has the status WAKING
				//setting the status to PLAYING
				catArray[cleoIndex].status = PLAYING;
				//cleo will play for max 7 seconds before being switched
				catArray[cleoIndex].playTime = 50;
				//os is done using resources
				mutex = false;
				//trigger the PendSV interrupt
				ICSR |= 1 << 28;
				__asm("isb");
			}
		}
	}
}				

//Changing a thread's state to sleep
void cleoSleep(int userSleepTime)
{
	catArray[cleoIndex].status = SLEEPING; //set the thread to sleep
	catArray[cleoIndex].sleepTime = userSleepTime; //Cleo will sleep for 5 seconds before waking
	osYield(); //call yield to run next thread
}
