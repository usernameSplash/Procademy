#include "EchoClient.h"

int wmain(void)
{
	EchoClient client;

	while (client.IsRunning())
	{
		Sleep(500);

		if (GetAsyncKeyState(0x57)) // W Key
		{
			client.Shutdown();
			break;
		}
	}

	client.Terminate();

	return 0;
}