#include <cstdio>
#include <winsock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 20000

int main(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	SOCKET listenSocket;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(SOCKADDR_IN);

	int retVal;	// to check error

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		printf("Error Code : %d", errorCode);

		WSACleanup();
		return 1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);

	retVal = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		printf("Error Code : %d", errorCode);

		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	
	retVal = listen(listenSocket, SOMAXCONN_HINT(SOMAXCONN));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		printf("Error Code : %d", errorCode);

		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		//clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);

		//if (clientSocket == INVALID_SOCKET)
		//{
		//	int errorCode = WSAGetLastError();
		//	printf("Error Code : %d", errorCode);

		//	closesocket(listenSocket);
		//	WSACleanup();
		//	return 1;
		//}
		//WCHAR clientIP[16] = { 0 };
		//InetNtop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));

		//printf("[TCP Server] Client Connected: IP Address = %ls, Port = %d\n\n", clientIP, ntohs(clientAddr.sin_port));

		//closesocket(clientSocket);
	}


	closesocket(listenSocket);

	WSACleanup();
	return 0;
}