#include "_threadsCore.h"

uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0;
	return (uint32_t*) *ptrmsp;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	unsigned int mspval = (unsigned int)getMSPInitialLocation();
	unsigned int pspval = mspval - offset;
	if (pspval%8!=0 || offset>2000)	//Max size of a stack defined as 2000
	{
		return 0;
		//pspval = pspval + (8 - (pspval%8));
	}
	return (unsigned int*)pspval;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	uint32_t pspval = (uint32_t) threadStack;
	__set_PSP(pspval);
	__set_CONTROL(0x02);	//Set to threading mode
}