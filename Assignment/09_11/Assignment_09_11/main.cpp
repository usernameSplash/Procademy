#include <cstdio>
#include <Windows.h>

#include "LockFreeStack.h"

unsigned int WINAPI LockFreeStackTest(void* arg);
LockFreeStack<int> g_Stack;

int wmain(void)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int threadCnt = (int)si.dwNumberOfProcessors - 2;

	HANDLE threads[50];

	for (int i = 0; i < threadCnt; i++)
	{
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, LockFreeStackTest, nullptr, 0, nullptr);
		if (threads[i] == NULL)
		{
			__debugbreak();
		}
	}

	WaitForMultipleObjects(threadCnt, threads, true, INFINITE);

	delete[] threads;

	return 0;
}

unsigned int WINAPI LockFreeStackTest(void* arg)
{
	while (true)
	{
		for (int iCnt = 0; iCnt < 100000; ++iCnt)
		{
			g_Stack.Push(iCnt);
		}

		for (int iCnt = 0; iCnt < 100000; ++iCnt)
		{
			g_Stack.Pop();
		}
	}

	return 0;
}