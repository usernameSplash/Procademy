#define PROFILE

#include "Job.h"
#include "Profiling.h"


void IterateMillionTimes(void)
{
	for (size_t iCnt = 0; iCnt < 1000000; iCnt++);
	return;
}

void IterateHundredTimes(void)
{
	for (size_t iCnt = 0; iCnt < 100; iCnt++);
	return;
}

void FuncA(void)
{
	for (size_t iCnt = 0; iCnt < 100; iCnt++)
	{
		PRO_BEGIN("Iterate Million Times");
		IterateMillionTimes();
		PRO_END("Iterate Million Times");
	}
}

void FuncB(void)
{
	for (size_t iCnt = 0; iCnt < 30; iCnt++)
	{
		PRO_BEGIN("Iterate Million Times");
		IterateMillionTimes();
		PRO_END("Iterate Million Times");
	}
}

void FuncC(void)
{
	for (size_t iCnt = 0; iCnt < 1000000; iCnt++)
	{
		PRO_BEGIN("Iterate Hundred Times");
		IterateHundredTimes();
		PRO_END("Iterate Hundred Times");
	}
}