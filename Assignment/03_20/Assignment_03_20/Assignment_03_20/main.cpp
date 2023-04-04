#define  _WIN32_WINNT 0x0600

#include <cstdio>
#include <Windows.h>

#include "Timer.h"
#include "SceneManager.h"


int main(void)
{
	SceneManager* sceneManager = SceneManager::GetInstance();
	Timer* timer = Timer::GetInstance();

	const DWORD TIME_PER_FRAME = 1000 / 60;
	DWORD deltaTime;

	bool bQuitted = false;

	while (bQuitted == false)
	{
		bQuitted = sceneManager->Update();
		
		timer->Tick();
		deltaTime = timer->GetDeltaTime();

		if (deltaTime < TIME_PER_FRAME)
		{
			Sleep(TIME_PER_FRAME - deltaTime);
		}
	}

	return 0;
}
