#include "LogData.h"
#include "LockFreeQueue.h"

#include <cstdio>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM 10
#define LOOP_NUM 1000

unsigned int WINAPI StackTest(void* arg);
unsigned int WINAPI PoolTest(void* arg);

static LockFreeQueue<int> s_queue;


int wmain(void)
{
	HANDLE threads[THREAD_NUM];

	for (long long iCnt = 0; iCnt < THREAD_NUM; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, StackTest, (void*)iCnt, 0, NULL);
		if (threads[iCnt] == nullptr)
		{
			__debugbreak();
		}
	}

	WaitForMultipleObjects(THREAD_NUM, threads, TRUE, INFINITE);

	wprintf(L"Head Node : %lld\n", s_queue._head);
	wprintf(L"Tail Node : %lld\n", s_queue._tail);

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
			s_queue.Enqueue(num);
		}

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			s_queue.Dequeue();
		}

		//if (repeatCnt % 10000 == 0)
		//{
		//	wprintf(L"%d\n", repeatCnt);
		//}
	}

	wprintf(L"%d Thread End\n", GetCurrentThreadId());

	return 0;
}
