#include <cstdio>
#include <ctime>
#include <Windows.h>
#include <timeapi.h>

#pragma comment(lib, "winmm.lib")

void FPS(void)
{
	static DWORD oldTick = timeGetTime();
	static DWORD frame = 0;

	frame++;

	if (timeGetTime() - oldTick >= 1000)
	{
		printf("FPS : %u\n", frame);
		frame = 0;
		oldTick += 1000;
	}
}

void FixedUpdate(void)
{
	DWORD curTick = 0;
	DWORD oldTick = timeGetTime();
	int deltaTick;

	while (true)
	{
		curTick = timeGetTime();
		deltaTick = curTick - oldTick;

		FPS();

		if (deltaTick <= 20)
		{
			Sleep(20 - deltaTick);
		}

		oldTick += 20;
		//oldTick = timeGetTime();
	}
}
int main(void)
{
	//timeBeginPeriod(1);

	FixedUpdate();

	return 0;
}