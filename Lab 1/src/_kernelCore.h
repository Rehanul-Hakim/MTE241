#ifndef CLEO_kernel
#define CLEO_kernel

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void);

//initialize anything that the first thread needs before it gets going, and switches between PSP and MSP
void kernel_start(void);	

//this performs the task/context switching
int task_switch(void);

//Idle thread
void osIdleTask();

#endif
