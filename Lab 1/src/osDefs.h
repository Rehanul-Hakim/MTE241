//osDefs.h includes everything that _threadsCore.c and _kernelCore.c needs
#ifndef CLEO_osDefs
#define CLEO_osDefs

#include <LPC17xx.h>

//registers used for the interrupts
#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0XE000ED04

//Stack defines
#define threadStackSize 0x200
#define threadStackPool 0x2000

//Maximum number of threads to be 8, allocated statically
#define maxThreads 8

//Cleo cat thread state
#define WAKING 0 //awake, but not playing
#define PLAYING 1 //running and playing
#define SLEEPING 2 //sleeping but not playing yet
#define DESTROYED 3 //for use later, especially for threads that end. This indicates that a new thread COULD go here if it needs to
#define BLOCKED 4	//

//Thread data structure
//Defines a structure called “thread_struct”, with “cleoThread” as the alias for the struct.  
typedef struct thread_struct{
	void (*threadFunc)(void* args);	//threadFunc is the function pointer
	uint32_t* taskPointer;	//stack pointer for this task
	int status;	//Cleo cat thread state
}cleoThread;

extern cleoThread catArray[maxThreads]; //catArray is an array size maxThreads containing cleoThread
extern int cleoNums; //current number of threads created
extern int cleoPlaying; //number of threads currently running

void osYield(void);	//pre-loads memory with important information to avoid problem of bootstrapping (context switch)

//threadsCore funtions that both _kernelCore.c and _threadsCore.c need
//Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP(uint32_t* threadStack); 

#endif
