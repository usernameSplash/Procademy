#include "LogData.h"
#include "LockFreeStack.h"

#include <cstdio>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM 10
#define LOOP_NUM 1000

unsigned int WINAPI ThreadProc(void* arg);

static LockFreeStack<int> s_stack;

int wmain(void)
{
	HANDLE threads[THREAD_NUM];

	for (int iCnt = 0; iCnt < THREAD_NUM - 1; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL);
		if (threads[iCnt] == nullptr)
		{
			__debugbreak();
		}
	}

	threads[9] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void*)1, 0, NULL);
	if (threads[9] == nullptr)
	{
		__debugbreak();
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);

	wprintf(L"Stack Top Node : %lld\n", GET_PTR(s_stack._top));

	return 0;
}

unsigned int WINAPI ThreadProc(void* arg)
{
	wprintf(L"%d Thread Start\n", GetCurrentThreadId());
	long long a = reinterpret_cast<int>(arg);

	for(int repeatCnt = 0; repeatCnt < 1000000; ++repeatCnt)
	{
		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Push(iCnt);
		}

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Pop();
		}

		if (a == 1 && repeatCnt % 100 == 0)
		{
			wprintf(L"RepeatCnt : %d\n", repeatCnt);
		}
	}

	return 0;
}