#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#define _WINSOCKAPI_

#include <cstdio>
#include <timeapi.h>

#include "Logic.h"
#include "Renderer.h"

#define SERVER_PORT 3000

/*
	Extern Variables
*/
SOCKET clientSocket;
SOCKADDR_IN serverAddr;

int wmain(void)
{
	timeBeginPeriod(1);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	int retVal;
	linger l;
	u_long noBlockSocketOpt = 1;
	timeval selectModelTimeout;

	DWORD prevTime;
	DWORD curTime;
	DWORD deltaTime;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		goto INVALID_CLIENT_SOCKET;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serverAddr.sin_port = htons(SERVER_PORT);

	retVal = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	l.l_onoff = 1;
	l.l_linger = 0;
	
	retVal = setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	retVal = ioctlsocket(clientSocket, FIONBIO, &noBlockSocketOpt);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	selectModelTimeout.tv_sec = 0;
	selectModelTimeout.tv_usec = 0;

	BufferClear();
	curTime = timeGetTime();

	while (TRUE)
	{
		UINT8 recvBuf[16];
		UINT8 sendBuf[16];
		fd_set readSet;
		bool bShouldSend;

		FD_ZERO(&readSet);
		FD_SET(clientSocket, &readSet);

		retVal = select(0, &readSet, NULL, NULL, &selectModelTimeout);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			goto SOCKET_ERROR_OCCURRED;
		}

		if (FD_ISSET(clientSocket, &readSet))
		{
			while (TRUE)
			{
				retVal = recv(clientSocket, (char*)recvBuf, 16, 0);
				if (retVal == SOCKET_ERROR)
				{
					int errorCode = WSAGetLastError();
					if (errorCode == WSAEWOULDBLOCK)
					{
						break;
					}

					wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
					goto SOCKET_ERROR_OCCURRED;
				}
				ProcessPacket(recvBuf);
			}
		}
		
		bShouldSend = Update(sendBuf);
		if (bShouldSend == true)
		{
			retVal = send(clientSocket, (char*)sendBuf, 16, 0);
			if (retVal == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				if (errorCode == WSAEWOULDBLOCK)
				{
					break;
				}

				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto SOCKET_ERROR_OCCURRED;
			}
		}

		RenderObjects();
	}

SOCKET_ERROR_OCCURRED:
	closesocket(clientSocket);
INVALID_CLIENT_SOCKET:
	WSACleanup();
	return 0;
}