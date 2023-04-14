#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 20000

int main(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET clientSocket;
	SOCKADDR_IN sockAddr;
	int retVal;

	sockAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &sockAddr.sin_addr);
	sockAddr.sin_port = htons(SERVER_PORT);

	size_t i = 0;
	while (true)
	{
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET)
		{
			int errorCode = WSAGetLastError();
			printf("Error Code : %d", errorCode);

			WSACleanup();
			return 1;
		}

		retVal = connect(clientSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			printf("Error Code : %d", errorCode);

			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		printf("%zu\n", i);
		i++;

		linger a;
		a.l_onoff = 1;
		a.l_linger = 0;

		setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&a, sizeof(a));
		closesocket(clientSocket);

//		Sleep(1000);
	}

	WSACleanup();
	return 0;
}