#include "stdbool.h"
#ifndef CLEO_kernel
#define CLEO_kernel

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void);

//initialize anything that the first thread needs before it gets going, and switches between PSP and MSP
void kernel_start(void);	

//this performs the task/context switching
int task_switch(void);

//mutex functions
//create the mutex
int osMutexCreate(void);
//thread acquires mutex if resource is free or adds thread to "waiting" queue if resource is locked
bool osMutexAcquire(int mutexID); 
//once thread is done using resources, mutex is released
bool osMutexRelease(int mutexID);
//if resource is unavailable, perform context switch
//void blockSwitch(int mutexID);
#endif