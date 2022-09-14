#include "_threadsCore.h"

uint32_t* getMSPInitialLocation(void) 
{
	unsigned int* ptrmsp = (unsigned int*)0x0;
	return (uint32_t*) *ptrmsp;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	unsigned int mspval = (unsigned int)getMSPInitialLocation();
	
	//uint32_t mspval = getMSPInitialLocation();
	//unsigned int mspaddress = &ptrmsp;
	//unsigned int psp = mspaddress - offset;
	//usigned int* ptrpsp = (unsigned int)psp;
	
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	
}