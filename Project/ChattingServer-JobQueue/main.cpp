#include "ChattingServer.h"

#include <locale>

int wmain(void)
{
	_wsetlocale(LC_ALL, L"korean");

	ChattingServer server;

	if (server.Initialize() == false)
	{
		wprintf(L"Chatting Server Initialize Failed\n");
		return 0;
	}

	wprintf(L"Chatting Server On\n");

	while (server.IsAlive())
	{

	}

	wprintf(L"Chatting Server Off\n");
	return 0;
}