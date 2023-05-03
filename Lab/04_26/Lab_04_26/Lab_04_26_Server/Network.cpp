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

	int bindRet;
	int setSockOptRet;
	int ioctlSocketRet;
	int listenRet;

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

	bindRet = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (bindRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	l.l_onoff = 1;
	l.l_linger = 0;
	setSockOptRet = setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (setSockOptRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	noBlockSocketOpt = 1;
	ioctlSocketRet = ioctlsocket(listenSocket, FIONBIO, &noBlockSocketOpt);
	if (ioctlSocketRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SOCKET_ERROR_OCCURRED;
	}

	listenRet = listen(listenSocket, SOMAXCONN_HINT(SOMAXCONN));
	if (listenRet == SOCKET_ERROR)
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

		wprintf(L"\nReceive Data From %u Users\n", readSet.fd_count);
		for (int iCnt = 0; iCnt < readSet.fd_count; iCnt++)
		{
			SOCKET s = readSet.fd_array[iCnt];
			if (s == listenSocket)
			{
				continue;
			}

			wprintf(L"Receive Data From Client\n");
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
	int setSockOptRet;
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
	setSockOptRet = setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (setSockOptRet == SOCKET_ERROR)
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
	UINT8 recvBuf[16 * 32];
	int recvRet;

	void* packetPtr;
	const ePacketType* type;
	
	while (TRUE)
	{
		recvRet = recv(refClientSocket, (char*)recvBuf, 16 * 32, 0);

		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
<<<<<<< HEAD
				Player_t* pPlayer = &(it->second);
				pPlayer->x = packetMoveStar->xCoord;
				pPlayer->y = packetMoveStar->yCoord;
				SendBroadcast(pPlayer->id, (char*)&packetMoveStar);
=======
				wprintf(L"WOULDBLOCK\n");
				return;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client\n");
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
>>>>>>> 23df9171cfa817f45bd0dab14567b19d91b990e1
			}

			PlayerID_t deleteId = std::hash<SOCKET>()(refClientSocket);
			DeleteUser(deleteId);
			return;
		}

		wprintf(L"Received %d Bytes from Client(%u)\n", recvRet, static_cast<PlayerID_t>(std::hash<SOCKET>()(refClientSocket)));
		for (int packetCount = 0; packetCount < recvRet / 16; packetCount++)
		{
			packetPtr = recvBuf + packetCount * 16;

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
	}

	return;
}

void SendUnicast(PlayerID_t& playerId, char* msg)
{
	Player player;
	int sendRet;
	auto it = g_PlayerList.find(playerId);

	if (it == g_PlayerList.end())
	{
		return;
	}

	player = it->second;

	sendRet = send(player.clientSocket, msg, 16, 0);

	if (sendRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode == WSAEWOULDBLOCK)
		{
			return;
		}
		else if (errorCode == WSAECONNRESET)
		{
			wprintf(L"Disconnected By Client(%u) in Broadcasting\n", player.id);
		}
		else
		{
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		}
		DeleteUser(playerId);

		return;
	}

	wprintf(L"[Send Message] Type: % d, Dest ID : %u\n", *reinterpret_cast<ePacketType*>(msg), it->second.id);

	return;
}

void SendBroadcast(PlayerID_t* excludedPlayerId, size_t playerCount, char* msg)
{
	int sendRet;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		for (int iCnt = 0; iCnt < playerCount; ++iCnt)
		{
			if (it->first == excludedPlayerId[iCnt])
			{
				goto NEXT_LOOP;
			}
		}

		sendRet = send(it->second.clientSocket, msg, 16, 0);

		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				break;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client(%u) in Broadcasting\n", it->second.id);
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			}
			DeleteUser(const_cast<PlayerID_t&>(it->first));
		}
		else
		{
			wprintf(L"[Broadcast Message] Type: %d, Dest ID: %u\n", *reinterpret_cast<ePacketType*>(msg), it->second.id);
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
	int closeSocketRet;
	auto it = g_ShouldDisconnectPlayerList.find(playerId);

	if (it == g_ShouldDisconnectPlayerList.end())
	{
		return;
	}

	clientSocket = it->second.clientSocket;
	closeSocketRet = closesocket(clientSocket);

	if (closeSocketRet == SOCKET_ERROR)
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