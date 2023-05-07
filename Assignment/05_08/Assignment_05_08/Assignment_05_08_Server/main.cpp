#include "Network.h"

int wmain(void)
{
	int retVal;

	retVal = SocketInitialize();
	if (retVal == FALSE)
	{
		return 1;
	}

	MessageController();

	return 0;
}