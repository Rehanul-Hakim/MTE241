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
		//case 1
		osMutexAcquire(UART);
		printf("Thread 1.\n");
		osMutexRelease(UART);
		//case 2
		//osMutexAcquire(GV);
		//x++;
		//printf("Thread 1. x is: %d\n", x);
		//osMutexRelease(GV);
		osYield();
	}
}

//Creating task 2
void task2(void* args)
{
	while(1)
	{
		//case 1
		osMutexAcquire(UART);
		printf("Thread 2.\n");
		osMutexRelease(UART);
		//case 2
		//osMutexAcquire(GV);
		//osMutexAcquire(LED);
		//y++;
		//unsigned int setNum = x%47;
		//printf("Thread 2. Set LED to: %d\n", setNum);
		//setLED(setNum);
		//osMutexRelease(GV);
		//osMutexRelease(LED);
		osYield();
	}
}

//extern unsigned long counter;
//Creating task 3
void task3(void* args)
{
	while(1)
	{
		//case 1
		osMutexAcquire(UART);
		printf("Thread 3.\n");
		osMutexRelease(UART);
		//case 2
		//osMutexAcquire(GV);
		//osMutexAcquire(LED);
		//z++;
		//int setNum = 0x71;
		//printf("Thread 3. Set LED to: %d\n", setNum);
		//setLED(setNum);
		//osMutexRelease(GV);
		//osMutexRelease(LED);
		osYield();
	}
}

//This is C. The expected function heading is int main(void).
int main( void ) 
{
	//Configure SysTick to generate an interrupt every millisecond
	SysTick_Config(SystemCoreClock/1000);
	
	//Always call this function at the start. It sets up various peripherals, the clock etc
	SystemInit();

	//Printf now goes to the UART
	printf("Hello, world!\r\n");
	
	//set LED directions
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
	LPC_GPIO2->FIODIR |= 1<<5;
	LPC_GPIO2->FIODIR |= 1<<6;
	
	//Getting our initial stack location
	unsigned int* mspval = getMSPInitialLocation();
	
	//Check what initial stack location is:
	printf("MSP initially: %x\n",(uint32_t)mspval);
	
	//Initialize the kernel
	kernelInit();
	
	//Setting up the threads
	createThread(task1);
	createThread(task2);
	createThread(task3);
	
	//Setting up the mutexes
	LED = osMutexCreate();
	UART = osMutexCreate();
	GV = osMutexCreate();
	
	//Start the kernel, which will run the first thread
	kernel_start();
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird.
	while(1);
}