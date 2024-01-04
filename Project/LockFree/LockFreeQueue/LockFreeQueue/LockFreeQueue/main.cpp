#include "LogData.h"
#include "LockFreeQueue.h"

#include <cstdio>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM (10)
#define REPEAT_NUM (1000000)
#define LOOP_NUM (3)
#define VALUE_KEY (0xff)

void QueueTest(int threadNum);
unsigned int WINAPI QueueTestWorkerThread(void* arg);

int wmain(void)
{
	QueueTest(THREAD_NUM);
}

void QueueTest(int threadNum)
{
	LockFreeQueue<int>* queue = new LockFreeQueue<int>(threadNum * LOOP_NUM);

	HANDLE* threads = new HANDLE[threadNum];

	for (int iCnt = 0; iCnt < threadNum; ++iCnt)
	{
		threads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, QueueTestWorkerThread, (void*)queue, NULL, NULL);
		if (threads[iCnt] == NULL)
		{
			wprintf(L"# Begin Thread Failed");
			__debugbreak();
		}
	}

	WaitForMultipleObjects(threadNum, threads, TRUE, INFINITE);

	return;
}

unsigned int WINAPI QueueTestWorkerThread(void* arg)
{
	LockFreeQueue<int>* queue = (LockFreeQueue<int>*)arg;

	wprintf(L"Thread %d Start\n", GetCurrentThreadId());

	for (int repeatCnt = 0; repeatCnt < REPEAT_NUM; ++repeatCnt)
	{
		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			queue->Enqueue(VALUE_KEY + iCnt);
		}

		Sleep(0);

		for (int iCnt = 0; iCnt < LOOP_NUM; ++iCnt)
		{
			int value = queue->Dequeue();
			if (value >= VALUE_KEY + LOOP_NUM || value < VALUE_KEY)
			{
				__debugbreak();
			}
		}
	}

	wprintf(L"Thread %d End\n", GetCurrentThreadId());

	return 0;
}