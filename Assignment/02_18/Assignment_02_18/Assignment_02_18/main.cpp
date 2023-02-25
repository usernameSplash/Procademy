#define PROFILE

#include <iostream>
#include "Profiling.h"
#include "Job.h"

#define OUTPUT_FILE_NAME "output.txt"

int main(void)
{
	ProfileReset();

	for(size_t iCnt = 0; iCnt < 10; iCnt++)
	{
		PRO_BEGIN("Func A");
		FuncA();
		PRO_END("Func A");
	}

	for (size_t iCnt = 0; iCnt < 10; iCnt++)
	{
		PRO_BEGIN("Func B");
		FuncB();
		PRO_END("Func B");
	}

	for (size_t iCnt = 0; iCnt < 10; iCnt++)
	{
		PRO_BEGIN("Func C");
		FuncC();
		PRO_END("Func C");
	}

	ProfileDataOutText(OUTPUT_FILE_NAME);

	return 0;
}