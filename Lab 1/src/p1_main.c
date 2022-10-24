//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

//This file is our threading library
#include "_threadsCore.h"

//Include the kernel
#include "_kernelCore.h"

//Include OS Definitions
#include "osDefs.h"

/*
	Main is where the user of our RTOS API will create their threads. Three threads will be created and 
	global variables will be used to indicate that they are working.
*/

int x = 0;
int y = 0;

void task1(void* args)
{
	while(1)
	{
		x++;
		printf("In task 1. x is: %d\n", x);
		osYield();
	}
}
void task2(void* args)
{
	while(1)
	{
		y++;
		printf("In task 2. y is: %d\n", y);
		osYield();
	}
}
void osIdleTask(void* args)
{
	while(1)
	{
		printf("In task 0\n");
		osYield();
	}
}

//This is C. The expected function heading is int main(void).

int main( void ) 
{

	//Always call this function at the start. It sets up various peripherals, the clock etc. 
	SystemInit();

	//Printf now goes to the UART, so be sure to have PuTTY open and connected
	printf("Hello, world!\r\n");
	
	//Getting our initial stack location
	unsigned int* mspval = getMSPInitialLocation();
	
	//Check what initial stack locatio is:
	//printf("MSP initially: %x\n",(uint32_t)mspval);
	
	//test function 'uint32_t* getNewThreadStack(uint32_t offset)'
	//unsigned int* pspval = getNewThreadStack(512);
	//printf("%u\n",pspval);
	
	//test function 'setThreadingWithPSP(uint32_t* threadStack)'
	//setThreadingWithPSP(pspval);
	
	//Initialize the kernel
	kernelInit();
	
	//Setting up the threads
	createThread(task1);
	createThread(task2);
	createThread(osIdleTask);
	
	//Start the kernel, which will run the first thread
	kernel_start();
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird
	while(1);
}
