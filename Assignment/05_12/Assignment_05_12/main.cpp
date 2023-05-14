
#include "Network.h"
#include "Logic.h"

using namespace std;

int main(void)
{
	chrono::high_resolution_clock::time_point curTime;
	chrono::high_resolution_clock::time_point prevTime;
	chrono::high_resolution_clock::duration frameTime;

	if (SocketInitialize() == false)
	{
		return 1;
	}

	curTime = chrono::high_resolution_clock::now();

	g_DeltaTime = MAX_FRAME_TIME + 1ns;

	while (!g_bShutdown)
	{
		MessageController();
		PacketProc();
		LogicProc();

		prevTime = curTime;
		curTime = std::chrono::high_resolution_clock::now();
		frameTime = curTime - prevTime;

		g_DeltaTime = g_DeltaTime + frameTime;
	}

	Terminate();

	return 0;
}
