#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 3000

enum class ePacketType
{
	ASSIGN_ID = 0,
	CREATE_STAR,
	DELETE_STAR,
	MOVE_STAR
};

typedef struct PacketAssignID
{
	const ePacketType type = ePacketType::ASSIGN_ID;
	UINT32 id;
	UINT32 temp[2];
} PacketAssignID_t;

typedef struct PacketCreateStar
{
	const ePacketType type = ePacketType::CREATE_STAR;
	UINT32 id;
	UINT32 xCoord;
	UINT32 yCoord;
} PacketCreateStar_t;

typedef struct PacketDeleteStar
{
	const ePacketType type = ePacketType::DELETE_STAR;
	UINT32 id;
	UINT32 temp[2];
} PacketDeleteStar_t;

typedef struct PacketMoveStar
{
	const ePacketType type = ePacketType::MOVE_STAR;
	UINT32 id;
	UINT32 xCoord;
	UINT32 yCoord;
} PacketMoveStar_t;


/*
	Extern Variables
*/
SOCKET clientSocket;
SOCKADDR_IN serverAddr;

int wmain(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}

	int retVal;
	u_long noBlockSocketOpt = 1;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		goto INVALID_CLIENT_SOCKET;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = 123;
	serverAddr.sin_port = htons(SERVER_PORT);

	retVal = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		printf("Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	retVal = ioctlsocket(clientSocket, FIONBIO, &noBlockSocketOpt);
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		printf("Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	while (TRUE)
	{
		UINT8 recvBuf[16];
		fd_set readSet;
		timeval time;

		FD_ZERO(&readSet);
		FD_SET(clientSocket, &readSet);

		time.tv_sec = 0;
		time.tv_usec = 0;

		retVal = select(0, &readSet, NULL, NULL, &time);
		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			printf("Error : %d on %d\n", errorCode, __LINE__);
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

					printf("Error : %d on %d\n", errorCode, __LINE__);
					goto SOCKET_ERROR_OCCURRED;
				}
			// logic;
			}
		}
		break;
	}

SOCKET_ERROR_OCCURRED:
	closesocket(clientSocket);
INVALID_CLIENT_SOCKET:
	WSACleanup();
	return 0;
}