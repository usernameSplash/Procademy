#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <process.h>
#include <intrin.h>
#include <atomic>

#include <cstdio>


void Lock(int procId);
void Unlock(int procId);

unsigned int WINAPI ThreadProc(void* arg);

bool flag[2];
int turn;

int g_Value = 0;
int g_ProcValue[2] = { 0, 0 };


int wmain(void)
{
	HANDLE threadArr[2];

	int procId[2] = { 0, 1 };

	threadArr[0] = (HANDLE)_beginthreadex(nullptr, 0, ThreadProc, (void*)&procId[0], 0, nullptr);
	threadArr[1] = (HANDLE)_beginthreadex(nullptr, 0, ThreadProc, (void*)&procId[1], 0, nullptr);

	WaitForMultipleObjects(2, threadArr, TRUE, INFINITE);

	wprintf(L"g_Value : %d\n", g_Value);
	wprintf(L"g_ProcValue1 : %d\n", g_ProcValue[0]);
	wprintf(L"g_ProcValue2 : %d\n", g_ProcValue[1]);

	return 0;
}

void Lock(int procId)
{
	int oppositeId = 1 - procId;
	flag[procId] = true;
	turn = oppositeId;

	_mm_mfence();
	//std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
	
	while (flag[oppositeId] && turn == oppositeId)
	{
	}

	return;
}

void Unlock(int procId)
{
	flag[procId] = false;
}

unsigned int WINAPI ThreadProc(void* arg)
{
	int procId = (*(int*)arg);

	for (int iCnt = 0; iCnt < 1000000000; iCnt++)
	{
		Lock(procId);
		g_Value++;
		Unlock(procId);

		g_ProcValue[procId]++;
	}

	return 0;
}