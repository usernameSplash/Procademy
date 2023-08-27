#include "main.h"

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <cstdio>
#include <Windows.h>

#pragma comment(lib, "winmm.lib") 

bool g_bRunning = true;

int wmain(void)
{
	wprintf(L"# Server Begin\n");

	while (g_bRunning)
	{
		if (GetAsyncKeyState(0x51)) // Q Key
		{
			// Quit Server
		}
		else if (GetAsyncKeyState(VK_ESCAPE))
		{
			break;
		}

		Sleep(100);
	}

	wprintf(L"# Server End\n");

	return 0;
}