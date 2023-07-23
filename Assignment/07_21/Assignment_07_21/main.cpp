#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <process.h>

#include <cstdio>


void Lock0(void);
void Lock1(void);

void Unlock0(void);
void Unlock1(void);

unsigned int WINAPI ThreadProc1(void* arg);
unsigned int WINAPI ThreadProc2(void* arg);

bool flag[2];
int turn;

int g_Value = 0;
int g_ProcValue1 = 0;
int g_ProcValue2 = 0;

int g_Status = 0;

int wmain(void)
{
	HANDLE threadArr[2];

	threadArr[0] = (HANDLE)_beginthreadex(nullptr, 0, ThreadProc1, nullptr, 0, nullptr);
	threadArr[1] = (HANDLE)_beginthreadex(nullptr, 0, ThreadProc2, nullptr, 0, nullptr);

	WaitForMultipleObjects(2, threadArr, TRUE, INFINITE);

	wprintf(L"g_Value : %d\n", g_Value);
	wprintf(L"g_ProcValue1 : %d\n", g_ProcValue1);
	wprintf(L"g_ProcValue2 : %d\n", g_ProcValue2);

	return 0;
}

void Lock0(void)
{
	turn = 0;
	flag[0] = true;

	_mm_mfence();

	while (flag[0] && flag[1] && turn == 0)
	{
		if (g_Status == 0x11)
		{
			g_Value = 0;
			return;
		}
	}

	g_Status |= 0x01;

	return;
}

void Lock1(void)
{
	turn = 1;
	flag[1] = true;

	_mm_mfence();

	while (flag[1] && flag[0] && turn == 1)
	{
		if (g_Status == 0x11)
		{
			g_Value = 0;
			return;
		}
	}
	
	g_Status |= 0x10;


	return;
}

void Unlock0(void)
{
	flag[0] = false;
}

void Unlock1(void)
{
	flag[1] = false;
}

unsigned int WINAPI ThreadProc1(void* arg)
{
	for (int iCnt = 0; iCnt < 100000000; iCnt++)
	{
		Lock0();
		g_Value++;
		g_Status &= 0x10;
		Unlock0();

		g_ProcValue1++;
	}

	return 0;
}

unsigned int WINAPI ThreadProc2(void* arg)
{
	for (int iCnt = 0; iCnt < 100000000; iCnt++)
	{
		Lock1();
		g_Value++;
		g_Status &= 0x01;
		Unlock1();

		g_ProcValue2++;
	}

	return 0;
}