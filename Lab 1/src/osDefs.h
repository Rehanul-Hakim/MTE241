//osDefs.h includes everything that _threadsCore.c and _kernelCore.c needs
#ifndef CLEO_osDefs
#define CLEO_osDefs

#include <LPC17xx.h>
#include "stdbool.h"

//registers used for the interrupts
#define SHPR3 *(uint32_t*)0xE000ED20
#define SHPR2 *(uint32_t*)0xE000ED1C
#define ICSR *(uint32_t*)0XE000ED04

//Stack defines
#define threadStackSize 0x200
#define threadStackPool 0x2000

//Maximum number of threads to be 8, allocated statically
#define maxThreads 8

//Maximum number of mutexes to be 8, allocated statically
#define maxMutex 8

//Time Cleo will play before preemptive switching
#define cleoPlayTime 500

//variable for yield in svc handler
#define YIELD_SWITCH 0
#define BLOCK_SWITCH 1

//Cleo cat thread state
#define WAKING 0 //awake, and ready to play (but not yet playing)
#define PLAYING 1 //running and playing
#define SLEEPING 2 //sleeping until timer is done, then goes to WAKING
#define DESTROYED 3 //for use later, especially for threads that end. This indicates that a new thread COULD go here if it needs to
#define BLOCKED 4	///thread cannot run until resources are available

//Thread data structure
//Defines a structure called thread_struct, with cleoThread as the alias for the struct.  
typedef struct thread_struct{
	void (*threadFunc)(void* args);	//threadFunc is the function pointer
	uint32_t* taskPointer;	//stack pointer for this task
	int status;	//Cleo cat thread state
	int playTime; //time in ms the thread is allowed to play before forcing yield (pre-emptive scheduling)
	int sleepTime; //time in ms that Cleo sleeps before waking
}cleoThread;

//Mutex data structure
//Defines a structure called mutex_struct, with cleoMutex as the alias for the struct.  
typedef struct mutex_struct{
	bool cleoResource; //Boolean to indicate if resource is available or not
	int resourceID; //Mutex ID
	int threadOwner; //Index of the thread which currently owns the mutex
	int waitingQueue[maxMutex]; //Waiting queue of threads
	int back;	//Back of queue
	int front; //Front of queue
}cleoMutex;

//waiting queue functions
//check if queue is empty
int isEmpty(int mutexID);
//to put a thread into a queue
void enqueue(int waitingIndex, int mutexID);
//take a thread out of a queue, returns true if success, false if queue empty
int dequeue(int mutexID);

//external variables
extern cleoThread catArray[maxThreads]; //catArray is an array size maxThreads containing cleoThread
extern int cleoNums; //current number of threads created
extern int cleoIndex; //the index of the current running thread
//types of mutexes
extern int LED;
extern int UART;
extern int GV;

//threadsCore funtions that both _kernelCore.c and _threadsCore.c need
void setThreadingWithPSP(uint32_t* threadStack); //Sets value of PSP to threadStack and changes the CONTROL register
void osYield();	//pre-loads memory with important information to avoid problem of bootstrapping (context switch)
void osIdleTask(); //Idle thread
int createThread (void (*task)(void* args)); //Sets up the threads

void SVC_Handler_Main(uint32_t *svc_args); //get the value of the system call's immediate
void cleoScheduler(); //determine which task to run next
#endif