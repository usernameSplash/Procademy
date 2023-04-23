#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_ADDR L"127.0.0.1"
#define SERVER_PORT 10099

int wmain(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET clientSocket;
	SOCKADDR_IN serverAddress;
	int retVal;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 1;
	}

	serverAddress.sin_family = AF_INET;
	InetPton(AF_INET, SERVER_ADDR, &serverAddress.sin_addr);
	serverAddress.sin_port = htons(SERVER_PORT);

	retVal = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d\n", errorCode);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		WCHAR buf[256];
		size_t len;

		wprintf(L"Message: ");
		
		if (fgetws(buf, 256, stdin) == NULL)
		{
			break;
		}

		len = wcslen(buf);

		if (len == 1)
		{
			break;
		}

		if (buf[len - 1] == '\n')
		{
			buf[len - 1] = '\0';
		}

		retVal = send(clientSocket, (char*)buf, len * 2 + 2, 0);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d\n", errorCode);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		retVal = recv(clientSocket, (char*)buf, retVal, MSG_WAITALL);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d\n", errorCode);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
		else if (retVal == 0)
		{
			break;
		}

		buf[retVal] = '\0';
		wprintf(L"[%d Bytes Received From Server]\n", retVal);
		wprintf(L"[Received Message] : %s\n", buf);
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}