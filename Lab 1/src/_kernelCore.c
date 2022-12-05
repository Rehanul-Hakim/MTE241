#include "osDefs.h"
#include "_kernelCore.h"
#include "stdio.h"
#include <stdint.h>

//the index of the mutex
int mutexIndex = 0;
//number of mutexes existing
int mutexNums = 0;

//Types of Mutexes
int LED; //Mutex to protect use of LEDs
int UART; //Mutex to protect UART functions like "printf"
int GV; //Mutex to protect use of global variables

//Array of structs to store mutexes.
//mutexArray is an array of size maxMutex containing cleoMutex
cleoMutex mutexArray[maxMutex]; 

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

//cleoScheduler determines which task to run next
void cleoScheduler()
{
	//variable to store the original index
	int originalIndex = cleoIndex;
	//go to the next task in the array, if we are at the end, loop back to 
	//the beginning
	cleoIndex = (cleoIndex+1)%(cleoNums);
	//if the thread we are at is not waking, then go to the next thread
	//also check if we already checked the entire array and we are back at the beginning
	while (catArray[cleoIndex].status != WAKING && originalIndex != cleoIndex){
		cleoIndex = (cleoIndex+1)%(cleoNums);				
	}
	//if all threads are sleeping OR blocked, then run the idle task thread
	if (originalIndex == cleoIndex && catArray[cleoIndex].status == SLEEPING)
	{
		//set the index to where the idle task is to run idle
		//the idle task is always at the end of the array
		cleoIndex = cleoNums - 1;
		catArray[cleoIndex].status = PLAYING;
	}
	else if (originalIndex == cleoIndex && catArray[cleoIndex].status == BLOCKED)
	{
		//set the index to where the idle task is to run idle
		//the idle task is always at the end of the array
		cleoIndex = cleoNums - 1;
		catArray[cleoIndex].status = PLAYING;
	}
	//if a thread is found that has the status WAKING
	else {
		//setting the status to PLAYING
		catArray[cleoIndex].status = PLAYING;
		//cleo will play for max 500 ms seconds before being switched
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
			catArray[cleoIndex].status = WAKING;	
			//moving the task pointer down to allocate space for 8 registers to be 
			//stored by the handler
			catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 8*4);
		}
		//run the scheduler
		cleoScheduler();
		//trigger the PendSV interrupt
		ICSR |= 1 << 28;
		__asm("isb");
	}
	else if (call == BLOCK_SWITCH)
	{
		//the status of this thread is now blocked
		catArray[cleoIndex].status = BLOCKED;
		//save registers and go to next task
		catArray[cleoIndex].taskPointer = (uint32_t*)(__get_PSP() - 8*4);
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

//Idle thread that will run when either all threads are sleeping, blocked, or there are no threads
void osIdleTask()
{
	while (1) {
		osYield();
	}
}

//create the mutex, returns the ID of the mutex
//allocates memory if needed, sets up mutex and allows OS to assign ownership
int	osMutexCreate(void)
{	
	if (mutexNums < maxMutex)
	{
		//set the resource to be initially available
		mutexArray[mutexNums].cleoResource = true;
		//set the ID to the value of the index
		mutexArray[mutexNums].resourceID = mutexNums;
		//make a new waiting queue for the mutex
		mutexArray[mutexNums].front = -1;
		mutexArray[mutexNums].back = -1;
		//adding new mutex to array
		mutexNums++;
		//return the ID of the mutex just created
		return mutexNums-1;
	} 
	//return error if the array is full
	return -1;
}

//thread acquires mutex if resource is free or adds thread to "waiting" queue if resource is locked
void osMutexAcquire(int mutexID)
{
	//check if the thread already owns this mutex
	if (mutexArray[mutexID].threadOwner != cleoIndex)
	{
		//mutexArray[mutexID].cleoResource = false;
		//if the resource is available for use
		if (mutexArray[mutexID].cleoResource == true) 
		{
			//assign the ownership of that mutex to the thread that wants it
			mutexArray[mutexID].threadOwner = cleoIndex;
			//thread is now using this mutex
			mutexArray[mutexID].cleoResource = false;
		}
		else 
		{
		enqueue(cleoIndex, mutexID);
		//Trigger the SVC to switch
		__ASM("SVC #1");
		}
	}
}

//once thread is done using resources, mutex is released
void osMutexRelease(int mutexID)
{
	if (mutexArray[mutexID].threadOwner == cleoIndex) 
	{
		mutexArray[mutexID].cleoResource = true;
		//check if the queue is not empty
		if (!isEmpty(mutexID)) {
			//release the front thread
			//set the status to active
			catArray[dequeue(mutexID)].status = WAKING;
		}
	}
}

//waiting queue functions (circular queue)
//check if waiting queue is empty
int isEmpty(int mutexID) {
  if (mutexArray[mutexID].front == -1) {
		return 1;
	}
  return 0;
}

//put a thread into a circular queue
void enqueue(int waitingIndex, int mutexID)
{
	//we don't have a full check since no more than 8 threads can be added
	//to this array anyway
	//if its the first thread in the array
	if (mutexArray[mutexID].front == - 1)
	{
		mutexArray[mutexID].front = 0;
	}
	//increase the back value as we add a new thread
	//loop back to the beginning if we reached the end index
	mutexArray[mutexID].back = (mutexArray[mutexID].back + 1) % cleoNums;
	//put the index of the waiting thread into the array
	mutexArray[mutexID].waitingQueue[mutexArray[mutexID].back] = waitingIndex;
} 

//take thread out of the array fifo style
int dequeue(int mutexID)
{
	//if we are trying to take a thread out of an empty waiting dequeue
  if (mutexArray[mutexID].front == - 1)
  {
		//if empty, return error
		return -1;
  }
	// the index of the thread that is returned
	int returnedCleo;
	returnedCleo = mutexArray[mutexID].waitingQueue[mutexArray[mutexID].front];
	//if queue is empty
	if (mutexArray[mutexID].front == mutexArray[mutexID].back) {
		mutexArray[mutexID].front = -1;
		mutexArray[mutexID].back = -1;
	} 
	// if the queue is not empty, move front index forward
	else {
		mutexArray[mutexID].front = (mutexArray[mutexID].front + 1) % cleoNums;
	}
	return returnedCleo;
} 
