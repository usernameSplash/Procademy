#include "Network.h"
#include "Player.h"
#include "Packet.h"

#include <utility>

std::unordered_map<PlayerID_t, Player> g_ShouldDisconnectPlayerList;
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
		//UINT8 sendBuf[16];
		timeval selectModelTimeout;
		selectModelTimeout.tv_sec = 0;
		selectModelTimeout.tv_usec = 10000;

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
			wprintf(L"Accept New Client\n");
			AcceptProc();
		}

		for (int iCnt = 0; iCnt < readSet.fd_count; iCnt++)
		{
			SOCKET s = readSet.fd_array[iCnt];
			if (s == listenSocket)
			{
				continue;
			}

			wprintf(L"New Packet From Client\n");
			RecvProc(s);
		}

		DisconnectPlayers();
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
		goto ACCEPT_FAILED;
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

	g_PlayerList.insert(std::make_pair(newPlayer.id, newPlayer));

	SendUnicast(newPlayer.id, (char*)&packetAssignID);
	
	SendUnicast(newPlayer.id, (char*)&packetCreateStar);
	SendBroadcast(&newPlayer.id, 1, (char*)&packetCreateStar);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->first == newPlayer.id)
		{
			continue;
		}

		packetCreateStar.id = it->first;
		packetCreateStar.xCoord = it->second.x;
		packetCreateStar.yCoord = it->second.y;
		SendUnicast(newPlayer.id, (char*)&packetCreateStar);
	}

	wprintf(L"Create New Client Success\n");

LINGER_SET_FAILED:
ACCEPT_FAILED:
	return;
}

void RecvProc(SOCKET& refClientSocket)
{
	UINT8 recvBuf[16];
	int retVal;

	void* packetPtr;
	const ePacketType* type;
	
	while (TRUE)
	{
		retVal = recv(refClientSocket, (char*)recvBuf, 16, 0);

		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				wprintf(L"WOULDBLOCK");
				return;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client\n");
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			}

			PlayerID_t deleteId = std::hash<SOCKET>()(refClientSocket);
			DeleteUser(deleteId);
			return;
		}

		packetPtr = recvBuf;
		type = static_cast<ePacketType*>(packetPtr);

		switch (*type)
		{
		case ePacketType::MOVE_STAR:
			{
				PacketMoveStar_t* packetMoveStar = static_cast<PacketMoveStar_t*>(packetPtr);
				packetMoveStar->type = ePacketType::MOVE_STAR;

				auto it = g_PlayerList.find(packetMoveStar->id);

				if (it == g_PlayerList.end())
				{
					break;
				}

				Player_t* pPlayer = &(it->second);
				pPlayer->x = packetMoveStar->xCoord;
				pPlayer->y = packetMoveStar->yCoord;
				wprintf(L"[Move Star] ID: %u, x:%d, y:%d\n", pPlayer->id, pPlayer->x, pPlayer->y);

				SendBroadcast(&pPlayer->id, 1, (char*)packetMoveStar);

				break;
			}
		default:
			break;
		}
	}

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
	wprintf(L"[Send Message] Type: % d, Dest ID : % u\n", *reinterpret_cast<ePacketType*>(msg), it->second.id);

	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			DeleteUser(playerId);
		}
	}

	return;
}

void SendBroadcast(PlayerID_t* excludedPlayerId, size_t playerCount, char* msg)
{
	int retVal;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		for (int iCnt = 0; iCnt < playerCount; ++iCnt)
		{
			if (it->first == excludedPlayerId[iCnt])
			{
				goto NEXT_LOOP;
			}
		}

		retVal = send(it->second.clientSocket, msg, 16, 0);
		wprintf(L"[Broadcast Message] Type: %d, Dest ID: %u\n", *reinterpret_cast<ePacketType*>(msg), it->second.id);

		if (retVal == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				break;
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				DeleteUser(const_cast<PlayerID_t&>(it->first));
			}
		}

	NEXT_LOOP:
		continue;
	}

	return;
}

void DeleteUser(PlayerID_t& playerId)
{
	if (g_PlayerList.find(playerId) != g_PlayerList.end())
	{
		g_ShouldDisconnectPlayerList.insert(std::make_pair(playerId, g_PlayerList.find(playerId)->second));
		g_PlayerList.erase(playerId);
	}

	return;
}

void Disconnect(PlayerID_t& playerId)
{
	SOCKET clientSocket;
	int retVal;
	auto it = g_ShouldDisconnectPlayerList.find(playerId);

	if (it == g_ShouldDisconnectPlayerList.end())
	{
		return;
	}

	clientSocket = it->second.clientSocket;
	retVal = closesocket(clientSocket);

	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK)
		{
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			return;
		}
	}

	wprintf(L"Disconnect ID : %u\n", playerId);

	PacketDeleteStar_t packetDeleteStar;

	packetDeleteStar.type = ePacketType::DELETE_STAR;
	packetDeleteStar.id = playerId;

	SendBroadcast(NULL, 0, (char*)&packetDeleteStar);

	return;
}

void DisconnectPlayers(void)
{
	if (!g_ShouldDisconnectPlayerList.empty())
	{
		wprintf(L"Disconnect %zu Players\n", g_ShouldDisconnectPlayerList.size());
	}

	for (auto it = g_ShouldDisconnectPlayerList.begin(); it != g_ShouldDisconnectPlayerList.end(); ++it)
	{
		Disconnect(const_cast<PlayerID_t&>(it->first));
	}

	g_ShouldDisconnectPlayerList.clear();
}