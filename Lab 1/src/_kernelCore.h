#ifndef CLEO_kernel
#define CLEO_kernel

void kernelInit(void);	//initializes memory structures and interrupts necessary to run the kernel
void osSched(void);	//called by the kernel to schedule which threads to run
void kernel_start(void);	//initialize anything that the first thread needs before it gets going, and switches between PSP and MSP
int task_switch(void);

#endif
