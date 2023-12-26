#include "LogData.h"
#include "LockFreeStack.h"

#include <cstdio>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM 10
#define LOOP_NUM 1000

unsigned int WINAPI StackTest(void* arg);
unsigned int WINAPI PoolTest(void* arg);

static LockFreeStack<int> s_stack;
static LockFreePool<int> s_pool(10000);

__declspec(thread) int* arr[1000];	//worker thread test
int* arr2[10000];					// main thread test


int wmain(void)
{
	HANDLE threads[THREAD_NUM];

	for (int iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, StackTest, (void*)iCnt, 0, NULL);
		if (threads[iCnt] == nullptr)
		{
			__debugbreak();
		}
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);

	wprintf(L"Top Node : %lld\n", GET_PTR(s_stack._top));

	//for (int iCnt = 0; iCnt < 10000; iCnt++)
	//{
	//	arr2[iCnt] = s_pool.Alloc();
	//}

	//wprintf(L"Top Node : %lld\n", GET_PTR(s_pool._top));

	return 0;
}

unsigned int WINAPI StackTest(void* arg)
{
	wprintf(L"%d Thread Start\n", GetCurrentThreadId());
	long long num = reinterpret_cast<long long>(arg);

	for(int repeatCnt = 0; repeatCnt < 1000000; ++repeatCnt)
	{
		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Push(num);
		}

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_stack.Pop();
		}

		//if (repeatCnt % 10000 == 0)
		//{
		//	wprintf(L"%d\n", repeatCnt);
		//}
	}

	wprintf(L"%d Thread End\n", GetCurrentThreadId());

	return 0;
}

unsigned int WINAPI PoolTest(void* arg)
{
	wprintf(L"%d Thread Start\n", GetCurrentThreadId());
	long long num = reinterpret_cast<long long>(arg);

	for (int repeatCnt = 0; repeatCnt < 1000000; ++repeatCnt)
	{
		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			 arr[iCnt] = s_pool.Alloc();
			 *arr[iCnt] = (num << 8) + iCnt;
		}

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_pool.Free(arr[iCnt]);
		}

		//if (repeatCnt % 10000 == 0)
		//{
		//	wprintf(L"%d\n", repeatCnt);
		//}
	}

	wprintf(L"%d Thread End\n", GetCurrentThreadId());

	return 0;
}