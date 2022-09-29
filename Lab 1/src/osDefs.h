#include <LPC17xx.h>
#ifndef CLEO_osDefs
#define CLEO_osDefs

#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0XE000ED04
#define threadStackSize uint32_t 0x2000
#define maxRunningThread uint32_t 0x8	//Max number of threads that can be statically loaded

typedef struct thread_struct{
	uint32_t* threadStack;
	void (*threadFunc)(void* args);
}cleoThread;

void osYield(void);	//pre-loads memory with important information to avoid problem of bootstrapping (context switch)
//void kernel_initialize(void);	//changes various chip settings and initializes important memory
//void create_thread(void);	//initialize any thread-specific memory, then add a new thread to statically defined array

#endif
