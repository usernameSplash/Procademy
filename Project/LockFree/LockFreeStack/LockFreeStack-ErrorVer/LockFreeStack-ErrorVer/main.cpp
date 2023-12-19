#include "LogData.h"
#include "LockFreeStack.h"

#include <cstdio>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM 10
#define LOOP_NUM 100000

unsigned int WINAPI ThreadProc(void* arg);

static LockFreeStack<int> s_stack;

int wmain(void)
{
	HANDLE threads[THREAD_NUM];

	for (int iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
		if (threads[iCnt] == nullptr)
		{
			__debugbreak();
		}
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);

	wprintf(L"Stack Top Node : %p\n", s_stack._top);

	return 0;
}

unsigned int WINAPI ThreadProc(void* arg)
{
	wprintf(L"%d Thread Start\n", GetCurrentThreadId());

	for(int repeatCnt = 0; repeatCnt < 10000; ++repeatCnt)
	{
		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Push(iCnt);
		}

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Pop();
		}
	}

	return 0;
}