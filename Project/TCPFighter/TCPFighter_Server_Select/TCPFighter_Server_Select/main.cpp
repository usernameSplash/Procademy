#include "main.h"

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <cstdio>
#include <Windows.h>

#include "Server.h"

#pragma comment(lib, "winmm.lib") 

bool g_bRunning = true;

int wmain(void)
{
	wprintf(L"# Server Begin\n");

	TCPFighter_Select_Server::Server* server = new TCPFighter_Select_Server::Server;

	wprintf(L"# [ESC] : Quit Server\n");

	while (g_bRunning)
	{
		server->IOController();
		server->Update();
		server->Monitor();

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			g_bRunning = false;
		}
	}

	delete server;

	wprintf(L"# Server End\n");

	return 0;
}