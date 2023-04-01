#define  _WIN32_WINNT 0x0600

#include <cstdio>
#include <Windows.h>
#include <timeapi.h>

#include "SceneManager.h"

#pragma comment(lib, "Winmm.lib") // for using timeGetTime function

int main(void)
{
	SceneManager* sceneManager = SceneManager::GetInstance();

	const DWORD TIME_PER_FRAME = 1000 / 60;
	timeBeginPeriod(1);

	DWORD prevTime;
	DWORD curTime;
	DWORD deltaTime;

	int iX = 0;  
	int iY = 0;

	curTime = timeGetTime();
	while (1)
	{
		sceneManager->Update();

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

	return 0;
}
