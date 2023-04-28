#include "Network.h"
#include "Player.h"

SOCKET listenSocket;
SOCKADDR_IN serverAddr;

#define SERVER_PORT 3000

int SocketInitialize(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return FALSE;
	}

	int retVal;
	linger l;
	u_long noBlockSocketOpt;

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto INVALID_SOCKET_ERROR;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_PORT);

	retVal = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	l.l_onoff = 1;
	l.l_linger = 0;
	retVal = setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	noBlockSocketOpt = 1;
	retVal = ioctlsocket(listenSocket, FIONBIO, &noBlockSocketOpt);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	retVal = listen(listenSocket, SOMAXCONN_HINT(SOMAXCONN));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	return TRUE;

SOCKET_ERROR_OCCURRED:
	closesocket(listenSocket);
INVALID_SOCKET_ERROR:
	WSACleanup();
	return FALSE;
}

void MessageController(void)
{
	int selectRet;

	while (TRUE)
	{
		UINT8 recvBuf[16];
		UINT8 sendBuf[16];
		fd_set readSet;

		FD_ZERO(&readSet);
		FD_SET(listenSocket, &readSet);

		selectRet = select(0, &readSet, NULL, NULL, NULL);

		if (selectRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			goto SELECT_FAILED;
		}

		if (FD_ISSET(listenSocket, &readSet))
		{
			AcceptProc();
		}
	}

SELECT_FAILED:
	closesocket(listenSocket);
	WSACleanup();
	return;
}

void AcceptProc(void)
{
	int serverAddrLen = sizeof(serverAddr);
	SOCKET clientSocket;

	clientSocket = accept(listenSocket, (SOCKADDR*)&serverAddr, &serverAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		wprintf(L"Create Client Socket Failed");
		goto SELECT_FAILED;
	}

SELECT_FAILED:
	closesocket(listenSocket);
	WSACleanup();
	return;
}