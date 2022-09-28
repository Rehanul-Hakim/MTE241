#ifndef CLEO_kernel
#define CLEO_kernel

#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0XE000ED04
void kernelInit(void);	//initializes memory structures and interrupts necessary to run the kernel
void osSched(void);	//called by the kernel to schedule which threads to run

#endif
