#include <cstdio>

#include "CrashDump.h"

CrashDump dump;

int wmain(void)
{
	int n;
	n = 10000;

	while (true)
	{
		wprintf(L"%d\n", 1000000 / n);
		n--;
	}
	return 0;
}