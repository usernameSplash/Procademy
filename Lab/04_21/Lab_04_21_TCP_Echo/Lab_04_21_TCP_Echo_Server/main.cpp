#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 10099

int wmain(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	SOCKET listenSocket;
	SOCKADDR_IN serverAddress;
	int retVal;

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		WSACleanup();
		return 1;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(SERVER_PORT);

	retVal = bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	retVal = listen(listenSocket, SOMAXCONN);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		SOCKET clientSocket;
		SOCKADDR_IN clientAddress;
		int clientAddressLen;
		clientAddressLen = sizeof(clientAddress);

		clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddress, &clientAddressLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d\n", errorCode);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		wprintf(L"[Connected Client Address : %d.%d.%d.%d:%d]\n", 
			clientAddress.sin_addr.S_un.S_un_b.s_b1,
			clientAddress.sin_addr.S_un.S_un_b.s_b2,
			clientAddress.sin_addr.S_un.S_un_b.s_b3,
			clientAddress.sin_addr.S_un.S_un_b.s_b4,
			ntohs(clientAddress.sin_port)
		);

		while (true)
		{
			WCHAR buf[256];
			size_t len;

			retVal = recv(clientSocket, (char*)buf, 255, 0);
			if (retVal == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"Error : %d\n", errorCode);
				closesocket(clientSocket);
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}
			else if (retVal == 0)
			{
				break;
			}

			buf[retVal] = '\0';

			wprintf(L"[%d Bytes Received From Client]\n", retVal);
			wprintf(L"[Received Message] : %s\n", buf);

			retVal = send(clientSocket, (char*)buf, retVal, 0);
			if (retVal == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"Error : %d\n", errorCode);
				closesocket(clientSocket);
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}
		}

		wprintf(L"Connection Terminated.\n\n");
		closesocket(clientSocket);
	}

	closesocket(listenSocket);
	WSACleanup();
	return 0;
}