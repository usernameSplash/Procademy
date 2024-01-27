#pragma once
#include <Windows.h>
#include <process.h>
#include <vector>
#include "ObjectPool.h"

#pragma comment(lib, "winmm.lib")

using namespace std;

#define TEST_CNT 2000
#define THREAD_CNT 8
#define NODE_MAX 20000
#define NANO_SEC 1000000000

double PoolAllocPerformanceResult[THREAD_CNT];
double PoolFreePerformanceResult[THREAD_CNT];

double HeapNewPerformanceResult[THREAD_CNT];
double HeapDeletePerformanceResult[THREAD_CNT];

vector<double> PoolAllocPerformance;
vector<double> PoolFreePerformance;

vector<double> HeapNewPerformance;
vector<double> HeapDeletePerformance;

vector<int> blockSize;

HANDLE threadBeginEvent;

template<typename T>
struct ThreadArgData
{
	ObjectPool<T>* _pool;
	int _idx;
};

template<typename T>
void PreheatPool(ObjectPool<T>* pool)
{
	T* dataBuf[TEST_CNT];

	for (int loopCnt = 0; loopCnt < 10; ++loopCnt)
	{
		for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
		{
			dataBuf[iCnt] = pool->Alloc();
		}

		for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
		{
			pool->Free(dataBuf[iCnt]);
		}
	}
}

template<typename T>
void PreheatHeap(void)
{
	T* dataBuf[TEST_CNT];

	for (int loopCnt = 0; loopCnt < 10; ++loopCnt)
	{
		for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
		{
			dataBuf[iCnt] = (T*)malloc(sizeof(T));
		}

		for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
		{
			free(dataBuf[iCnt]);
		}
	}

	return;
}

template<typename T>
unsigned int WINAPI ObjPoolThread(void* arg)
{
	ThreadArgData<T>* data = (ThreadArgData<T>*)arg;
	ObjectPool<T>* pool = data->_pool;
	int idx = data->_idx;
	
	LARGE_INTEGER freq;
	LARGE_INTEGER start;
	LARGE_INTEGER end;

	double allocMax = 0;
	double freeMax = 0;
	double duration;

	wprintf(L"ObjPool Thread %d Begin\n", idx);

	PreheatPool(pool);
	
	QueryPerformanceFrequency(&freq);

	PoolAllocPerformanceResult[idx] = 0;
	PoolFreePerformanceResult[idx] = 0;
	T* dataBuf[TEST_CNT];

	WaitForSingleObject(threadBeginEvent, INFINITE);

	for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
	{
		QueryPerformanceCounter(&start);
		dataBuf[iCnt] = pool->Alloc();
		QueryPerformanceCounter(&end);

		duration = (end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

		if (duration > allocMax)
		{
			allocMax = duration;
		}

		PoolAllocPerformanceResult[idx] += duration;
	}

	PoolAllocPerformanceResult[idx] -= allocMax;

	for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
	{
		QueryPerformanceCounter(&start);
		pool->Free(dataBuf[iCnt]);
		QueryPerformanceCounter(&end);

		duration = (end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

		if (duration > freeMax)
		{
			freeMax = duration;
		}

		PoolFreePerformanceResult[idx] += duration;
	}

	PoolFreePerformanceResult[idx] -= freeMax;

	return 0;
}

template<typename T>
unsigned int WINAPI HeapThread(void* arg)
{
	int idx = (int)arg;

	LARGE_INTEGER freq;
	LARGE_INTEGER start;
	LARGE_INTEGER end;

	double newMax = 0;
	double deleteMax = 0;
	double duration;

	wprintf(L"Heap Thread %d Begin\n", idx);

	QueryPerformanceFrequency(&freq);

	HeapNewPerformanceResult[idx] = 0;
	HeapDeletePerformanceResult[idx] = 0;
	T* dataBuf[TEST_CNT];

	PreheatHeap<T>();
	WaitForSingleObject(threadBeginEvent, INFINITE);

	for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
	{
		QueryPerformanceCounter(&start);
		dataBuf[iCnt] = (T*)malloc(sizeof(T));
		QueryPerformanceCounter(&end);

		duration = (end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

		if (duration > newMax)
		{
			newMax = duration;
		}

		HeapNewPerformanceResult[idx] += duration;
	}

	HeapNewPerformanceResult[idx] -= newMax;

	for (int iCnt = 0; iCnt < TEST_CNT; ++iCnt)
	{
		QueryPerformanceCounter(&start);
		free(dataBuf[iCnt]);
		QueryPerformanceCounter(&end);

		duration = (end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

		if (duration > deleteMax)
		{
			deleteMax = duration;
		}

		HeapDeletePerformanceResult[idx] += duration;
	}

	HeapDeletePerformanceResult[idx] -= deleteMax;

	return 0;
}

template<typename T>
void CompareTest(void)
{
	ObjectPool<T> pool(NODE_MAX);

	/*
		Alloc & Free
	*/

	HANDLE objPoolThread[THREAD_CNT];
	ThreadArgData<T> args[THREAD_CNT];
	ResetEvent(threadBeginEvent);
	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		args[iCnt]._pool = &pool;
		args[iCnt]._idx = iCnt;

		objPoolThread[iCnt] = (HANDLE)_beginthreadex(NULL, 0, ObjPoolThread<T>, &args[iCnt], 0, NULL);
	}
	SetEvent(threadBeginEvent);
	WaitForMultipleObjects(THREAD_CNT, objPoolThread, TRUE, INFINITE);

	double allocTotal = 0;
	double freeTotal = 0;
	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		allocTotal += PoolAllocPerformanceResult[iCnt];
		freeTotal += PoolFreePerformanceResult[iCnt];
	}

	allocTotal = (allocTotal * NANO_SEC) / (TEST_CNT * THREAD_CNT - THREAD_CNT);
	freeTotal = (freeTotal * NANO_SEC) / (TEST_CNT * THREAD_CNT - THREAD_CNT);

	PoolAllocPerformance.push_back(allocTotal);
	PoolFreePerformance.push_back(freeTotal);

	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		CloseHandle(objPoolThread[iCnt]);
	}

	/*
		New & Delete
	*/

	HANDLE heapThread[THREAD_CNT];

	ResetEvent(threadBeginEvent);
	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		heapThread[iCnt] = (HANDLE)_beginthreadex(NULL, 0, HeapThread<T>, (void*)iCnt, 0, NULL);
	}
	SetEvent(threadBeginEvent);
	WaitForMultipleObjects(THREAD_CNT, heapThread, TRUE, INFINITE);

	double newTotal = 0;
	double deleteTotal = 0;
	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		newTotal += HeapNewPerformanceResult[iCnt];
		deleteTotal += HeapDeletePerformanceResult[iCnt];
	}

	newTotal = (newTotal * NANO_SEC) / (TEST_CNT * THREAD_CNT - THREAD_CNT);
	deleteTotal = (deleteTotal * NANO_SEC) / (TEST_CNT * THREAD_CNT - THREAD_CNT);

	HeapNewPerformance.push_back(newTotal);
	HeapDeletePerformance.push_back(deleteTotal);

	for (int iCnt = 0; iCnt < THREAD_CNT; ++iCnt)
	{
		CloseHandle(heapThread[iCnt]);
	}

	blockSize.push_back(sizeof(T));

	return;
}

struct MyData
{
	char data[256];
};

struct MyData2
{
	char data[512];
};

void Test(void)
{
	threadBeginEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	timeBeginPeriod(1);
	
	CompareTest<int>();
	CompareTest<__int64>();
	CompareTest<MyData>();
	CompareTest<MyData2>();

	timeEndPeriod(1);

	for (int iCnt = 0; iCnt < blockSize.size(); ++iCnt)
	{
		wprintf(L"%d : %lf, %lf | %lf, %lf | Performance : %lf, %lf\n", blockSize[iCnt], PoolAllocPerformance[iCnt], PoolFreePerformance[iCnt], HeapNewPerformance[iCnt], HeapDeletePerformance[iCnt], HeapNewPerformance[iCnt] / PoolAllocPerformance[iCnt], HeapDeletePerformance[iCnt] / PoolFreePerformance[iCnt]);
	}
}