
#include "Network.h"
#include "Logic.h"

using namespace std;

int main(void)
{
	chrono::high_resolution_clock::time_point curTime;
	chrono::high_resolution_clock::time_point prevTime;
	chrono::high_resolution_clock::duration frameTime;

	if (SocketInitialize() == FALSE)
	{
		return 1;
	}

	curTime = chrono::high_resolution_clock::now();

	while (TRUE)
	{
		BOOL result;
		chrono::milliseconds sleepTime;

		result = RecvMessageController();
		if (result == FALSE)
		{
			break;
		}

		LogicProc();

		result = SendMessageController();
		if (result == FALSE)
		{
			break;
		}

		prevTime = curTime;
		curTime = std::chrono::high_resolution_clock::now();
		frameTime = curTime - prevTime;

		g_DeltaTime = g_DeltaTime + frameTime;

		sleepTime = chrono::duration_cast<chrono::milliseconds>(MAX_FRAME_TIME - frameTime);
		if (sleepTime.count() > 0)
		{
			Sleep((DWORD)sleepTime.count());
		}
	}

	Terminate();

	return 0;
}
