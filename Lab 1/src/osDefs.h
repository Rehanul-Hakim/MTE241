//osDefs.h includes everything that _threadsCore.c and _kernelCore.c needs
#ifndef CLEO_osDefs
#define CLEO_osDefs

#include <LPC17xx.h>
#include "stdbool.h"

//registers used for the interrupts
#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0XE000ED04

//Stack defines
#define threadStackSize 0x200
#define threadStackPool 0x2000

//Maximum number of threads to be 8, allocated statically
#define maxThreads 8

//Time Cleo will play before preemptive switching
#define cleoPlayTime 50;

//Cleo cat thread state
#define WAKING 0 //awake, and ready to play (but not yet playing)
#define PLAYING 1 //running and playing
#define SLEEPING 2 //sleeping until timer is done, then goes to WAKING
#define DESTROYED 3 //for use later, especially for threads that end. This indicates that a new thread COULD go here if it needs to
#define BLOCKED 4	///thread cannot run until resources are available


//Thread data structure
//Defines a structure called “thread_struct”, with “cleoThread” as the alias for the struct.  
typedef struct thread_struct{
	void (*threadFunc)(void* args);	//threadFunc is the function pointer
	uint32_t* taskPointer;	//stack pointer for this task
	int status;	//Cleo cat thread state
	int playTime; //time in ms the thread is allowed to play before forcing yield (pre-emptive scheduling)
	int sleepTime; //time in ms that Cleo sleeps before waking
}cleoThread;

extern cleoThread catArray[maxThreads]; //catArray is an array size maxThreads containing cleoThread
extern int cleoNums; //current number of threads created
extern int cleoPlaying; //number of threads currently running
extern int cleoIndex; //the index of the current running thread
extern bool mutex; //check if resources are available for interrupt

//threadsCore funtions that both _kernelCore.c and _threadsCore.c need
void setThreadingWithPSP(uint32_t* threadStack); //Sets value of PSP to threadStack and changes the CONTROL register
void osYield();	//pre-loads memory with important information to avoid problem of bootstrapping (context switch)
void osIdleTask(); //Idle thread
int createThread (void (*task)(void* args)); //Sets up the threads
#endif
