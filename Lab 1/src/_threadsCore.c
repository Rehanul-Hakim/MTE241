#include <LPC17xx.h>
#include "stdio.h"
#include "_threadsCore.h"

//the index of the task running
int cleoIndex = 0;
//number of threads existing
int cleoNums = 0;

//Array of structs to store threads.
//catArray is an array of size maxThreads containing cleoThread
cleoThread catArray[maxThreads]; 

//global counter variable that increments every ms
unsigned long counter = 0;

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0; //check 0x0 in table
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
		//function pointer to the task
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
		return cleoNums-1;
	} 
	return -1;
}

//Using SysTick for time dependant operations and preemptive switching
void SysTick_Handler(void)
{
	//increment the global counter variable every ms
	counter++;
	//using a for loop to go through the array and decrement any relevant
	//timers every ms and changes their status when appropriate
	int i;
	for (i = 0; i < cleoNums-1; ++i){
		//if the thread is sleeping and timer is not 0, decrement the sleep timer
		if (catArray[i].status == SLEEPING && catArray[i].sleepTime > 0) {
			catArray[i].sleepTime = catArray[i].sleepTime - 1;
		}
		//if the thread is sleeping and timer is 0, set the status to waking
		else if (catArray[i].status == SLEEPING && catArray[i].sleepTime == 0) {
			catArray[i].status = WAKING;
		}
		//if the thread is playing and timer is not 0, decrement the play timer
		else if (catArray[i].status == PLAYING && catArray[i].playTime > 0) {
			catArray[i].playTime = catArray[i].playTime - 1;
		}
		//if the thread is playing and timer is 0, only start performing the context
		else if (catArray[i].status == PLAYING && catArray[i].playTime == 0) {
			catArray[i].status = WAKING;	
			//moving the task pointer down to allocate space for 8 registers to be stored by the handler
			catArray[i].taskPointer = (uint32_t*)(__get_PSP() - 8*4);
			cleoScheduler();
			//trigger the PendSV interrupt
			ICSR |= 1 << 28;
			__asm("isb");
		}
	}
}				

//Changing a thread's state to sleep
void cleoSleep(int userSleepTime)
{
	catArray[cleoIndex].status = SLEEPING; //set the thread to sleep
	catArray[cleoIndex].sleepTime = userSleepTime; //Cleo will sleep for 5 seconds before waking
	//save registers and go to next task
	catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 16*4);
	cleoScheduler();
	//trigger the PendSV interrupt
	ICSR |= 1 << 28;
	__asm("isb");
}

//Functions to control LEDSz
//set LEDs
void setLED(unsigned int num)
{
	//clear all LEDS
	LPC_GPIO2->FIOCLR |= 1<<6;
	LPC_GPIO2->FIOCLR |= 1<<5;
	LPC_GPIO2->FIOCLR |= 1<<4;
	LPC_GPIO2->FIOCLR |= 1<<3;
	LPC_GPIO2->FIOCLR |= 1<<2;
	LPC_GPIO1->FIOCLR |= 1<<31;
	LPC_GPIO1->FIOCLR |= 1<<29;
	LPC_GPIO1->FIOCLR |= 1<<28;
	
	//set LEDs based on number
	if ((num&1) == 1){
		LPC_GPIO2->FIOSET |= 1<<6;
	}
	if ((num&2) == 2){
		LPC_GPIO2->FIOSET |= 1<<5;
	}
	if ((num&4) == 4){
		LPC_GPIO2->FIOSET |= 1<<4;
	}
	if ((num&8) == 8){
		LPC_GPIO2->FIOSET |= 1<<3;
	}
	if ((num&16) == 16){
		LPC_GPIO2->FIOSET |= 1<<2;
	}
	if ((num&32) == 32){
		LPC_GPIO1->FIOSET |= 1<<31;
	}
	if ((num&64) == 64){
		LPC_GPIO1->FIOSET |= 1<<29;
	}
	if ((num&128) == 128){
		LPC_GPIO1->FIOSET |= 1<<28;
	}
}