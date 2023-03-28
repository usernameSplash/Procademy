#define  _WIN32_WINNT 0x0600

#include <cstdio>
#include <memory.h>
#include <Windows.h>
#include <sysinfoapi.h>
#include <timeapi.h>

#include "Console.h"
#include "Logger.h"
#include "LoadData.h"
#include "Render.h"

#pragma comment(lib, "Winmm.lib") // for using timeGetTime function


int g_iX = 0;

int main(void)
{
	const DWORD TIME_PER_FRAME = 1000 / 60;
	timeBeginPeriod(1);

	DWORD prevTime;
	DWORD curTime;
	DWORD deltaTime;

	int iX = 0;  
	int iY = 0;

	cs_Initial();
	PrintLog("Program Start", error_t::NOTE);

	LoadAllStageFileName();
	curTime = timeGetTime();
	while (1)
	{

		iX++;
		iX = iX % dfSCREEN_WIDTH;
		iY++;
		iY = iY % dfSCREEN_HEIGHT;

		Buffer_Clear();
		Sprite_Draw(iX, iY, 'A');
		Buffer_Flip();

		prevTime = curTime;
		curTime = timeGetTime();

		if (curTime < prevTime)
		{
			deltaTime = MAXDWORD - prevTime + curTime;
		}
		else
		{
			deltaTime = curTime - prevTime;
		}

		if (deltaTime < TIME_PER_FRAME)
		{
			Sleep(TIME_PER_FRAME - deltaTime);
		}
	}

	PrintLog("Program End Successfully", error_t::NOTE);
	return 0;
}
