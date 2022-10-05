#ifndef CLEO_osDefs
#define CLEO_osDefs

#include <LPC17xx.h>

//registers used for the interrupts
#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0XE000ED04

//Stack defines
#define threadStackSize 0x215
#define threadStackPool uint32_t 0x2000

//Maximum number of threads to be 8, allocated statically
#define maxThreads 8

//cleo cat thread states
#define WAKING 0 //created, but not running
#define PLAYING 1 //running and playing
#define SLEEPING 2 //not running but ready to go
#define DESTROYED 3 //for use later, especially for threads that end. This indicates that a new thread COULD go here if it needs to

//Thread data structure
typedef struct thread_struct{
	void (*threadFunc)(void* args);
	//stack pointer for this task
	uint32_t* taskPointer;
	int status;
}cleoThread;

extern cleoThread catArray[maxThreads]; //catArray is an array size maxThreads containing cleoThread
extern int cleoNums; //current number of threads created
extern int cleoPlaying; //number of threads currently running

void osYield(void);	//pre-loads memory with important information to avoid problem of bootstrapping (context switch)

#endif
