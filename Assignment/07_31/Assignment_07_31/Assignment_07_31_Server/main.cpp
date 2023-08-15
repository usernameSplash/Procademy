#include "Network.h"
#include "Contents.h"

using namespace NetworkLibrary;

int wmain()
{
	Contents* content = new Contents();
	Network* network = new Network(content);

	while (network->IsRunning())
	{
		Sleep(500);

		if (GetAsyncKeyState(0x51)) // Q Key
		{
			network->Shutdown();
		}
	}

	return 0;
}