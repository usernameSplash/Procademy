#include "Network.h"
#include "Player.h"
#include "Packet.h"

#include <utility>

std::unordered_map<PlayerID_t, Player> g_PlayerList;

SOCKET listenSocket;
SOCKADDR_IN serverAddr;

fd_set readSet;
fd_set sendSet;

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

		FD_ZERO(&readSet);
		FD_SET(listenSocket, &readSet);

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			FD_SET(it->second.clientSocket, &readSet);
		}

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

		for (int iCnt = 0; iCnt < readSet.fd_count; iCnt++)
		{
			SOCKET s = readSet.fd_array[iCnt];
			if (s == listenSocket)
			{
				continue;
			}

			RecvProc(s);
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
	int retVal;
	SOCKET clientSocket;
	linger l;

	PacketAssignID_t packetAssignID;
	PacketCreateStar_t packetCreateStar;

	clientSocket = accept(listenSocket, (SOCKADDR*)&serverAddr, &serverAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		wprintf(L"Create Client Socket Failed");
		goto SELECT_FAILED;
	}

	l.l_onoff = 1;
	l.l_linger = 0;
	retVal = setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto LINGER_SET_FAILED;
	}

	Player newPlayer;
	newPlayer.clientSocket = clientSocket;
	newPlayer.id = std::hash<SOCKET>()(clientSocket);
	newPlayer.x = 40;
	newPlayer.y = 12;

	packetAssignID.type = ePacketType::ASSIGN_ID;
	packetAssignID.id = newPlayer.id;

	packetCreateStar.type = ePacketType::CREATE_STAR;
	packetCreateStar.id = newPlayer.id;
	packetCreateStar.xCoord = newPlayer.x;
	packetCreateStar.yCoord = newPlayer.y;

	SendUnicast(newPlayer.id, (char*)&packetAssignID);
	SendUnicast(newPlayer.id, (char*)&packetCreateStar);
	SendBroadcast(newPlayer.id, (char*)&packetCreateStar);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		packetCreateStar.id = it->first;
		packetCreateStar.xCoord = it->second.x;
		packetCreateStar.yCoord = it->second.y;
		SendUnicast(newPlayer.id, (char*)&packetCreateStar);
	}

	g_PlayerList.insert(std::make_pair(newPlayer.id, newPlayer));

LINGER_SET_FAILED:
SELECT_FAILED:
	return;
}

void RecvProc(SOCKET& refClientSocket)
{
	UINT8 recvBuf[16];
	int retVal;

	void* packetPtr;
	const ePacketType* type;
	
	retVal = recv(refClientSocket, (char*)recvBuf, 16, 0);

	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto RECV_FAILED;
	}
	else
	{
		//Disconnect(...);
	}

	packetPtr = recvBuf;
	type = static_cast<ePacketType*>(packetPtr);
	
	switch (*type)
	{
	case ePacketType::MOVE_STAR:
		{
			PacketMoveStar_t* packetMoveStar = static_cast<PacketMoveStar_t*>(packetPtr);

			auto it = g_PlayerList.find(packetMoveStar->id);

			if (it != g_PlayerList.end())
			{
				Player_t* pPlayer = &(it->second);
				pPlayer->x = packetMoveStar->xCoord;
				pPlayer->y = packetMoveStar->yCoord;
			}

			break;
		}
	default:
		break;
	}

RECV_FAILED:
	return;
}

void SendUnicast(PlayerID_t& playerId, char* msg)
{
	Player player;
	int retVal;
	auto it = g_PlayerList.find(playerId);

	if (it == g_PlayerList.end())
	{
		return;
	}

	player = it->second;

	retVal = send(player.clientSocket, msg, 16, 0);

	if (retVal == SOCKET_ERROR)
	{
		//Disconnect(playerId);
	}

	return;
}

void SendBroadcast(PlayerID_t& excludedPlayerId, char* msg)
{
	Player excludedPlayer;
	int retVal;
	auto it = g_PlayerList.find(excludedPlayerId);

	if (it == g_PlayerList.end())
	{
		return;
	}

	excludedPlayer = it->second;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->first == excludedPlayerId)
		{
			continue;
		}

		retVal = send(it->second.clientSocket, msg, 16, 0);

		if (retVal == SOCKET_ERROR)
		{
			//Disconnect(playerId);
		}
	}

	return;
}

void Disconnect(PlayerID_t& playerId)
{
	SOCKET clientSocket;
	int retVal;
	auto it = g_PlayerList.find(playerId);

	if (it == g_PlayerList.end())
	{
		return;
	}

	clientSocket = it->second.clientSocket;
	retVal = closesocket(clientSocket);

	if (retVal == SOCKET_ERROR)
	{
		//Disconnect(playerId);
	}

	return;
}