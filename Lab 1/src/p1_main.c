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
extern unsigned long counter;

//Creating task 1
void task1(void* args)
{
	while(1)
	{
		x++;
		printf("Meow 1 %d times %lu ms\n", x, counter);
		cleoSleep(1000);
	}
}

//Creating task 2
void task2(void* args)
{
	while(1)
	{
		y++;
		printf("Meow 2 %d times %lu ms\n", y, counter);
		cleoSleep(3000);
	}
}

//Creating task 3
void task3(void* args)
{
	while(1)
	{
		z++;
		printf("Meow 3 %d times %lu ms\n", z, counter);
		cleoSleep(5000);
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
	createThread(task1, 100);
	createThread(task2, 100);
	createThread(task3, 100);
	
	//Start the kernel, which will run the first thread
	kernel_start();
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird.
	while(1);
}
