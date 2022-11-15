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
int z = 0;

//Creating task 1
void task1(void* args)
{
	while(1)
	{
		x++;
		printf("In task 1. x is: %d\n", x);
		osYield();
	}
}

//Creating task 2
void task2(void* args)
{
	while(1)
	{
		y++;
		printf("In task 2. y is: %d\n", y);
		osYield();
	}
}
extern unsigned long counter;
//Creating task 3
void task3(void* args)
{
	while(1)
	{
		z++;
		//printf("In task 3. z is: %lu\n", counter);
		printf("In task 3. z is: %d\n", z);
		osYield();
	}
}

//This is C. The expected function heading is int main(void).
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc
	SystemInit();

	//Printf now goes to the UART
	printf("Hello, world!\r\n");
	
	//Getting our initial stack location
	unsigned int* mspval = getMSPInitialLocation();
	
	//Check what initial stack location is:
	printf("MSP initially: %x\n",(uint32_t)mspval);
	
	//Initialize the kernel
	kernelInit();
	
	//Setting up the threads
	createThread(task1, 10);
	createThread(task2, 7);
	createThread(task3, 2);
	
	//Start the kernel, which will run the first thread
	kernel_start();
	
	//Configure SysTick to generate an interrupt every millisecond
	SysTick_Config(SystemCoreClock/1000);
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird.
	while(1);
}
