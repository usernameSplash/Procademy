
#include <cstdlib>

#include "Network.h"
#include "Packet.h"

bool g_bShutdown = false;
std::unordered_map<PlayerID, Player> g_PlayerList;
static size_t s_CurDead = 0;

SOCKET listenSocket;
SOCKADDR_IN serverAddr;
fd_set readSet;
fd_set sendSet;

static PlayerID s_CurId = 0;

static timeval s_Time { 0,0 };

#define SERVER_PORT 5000

bool SocketInitialize(void)
{
	g_PlayerList.reserve(20);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
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

	ZeroMemory(&serverAddr, sizeof(serverAddr));
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

	wprintf(L"# Bind OK - Port : %d\n", SERVER_PORT);

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

	wprintf(L"# Listen OK\n");

	return true;

SOCKET_ERROR_OCCURRED:
	closesocket(listenSocket);
INVALID_SOCKET_ERROR:
	WSACleanup();
	return false;
}

void MessageController(void)
{
	int selectRet;

	FD_ZERO(&readSet);
	FD_ZERO(&sendSet);

	FD_SET(listenSocket, &readSet);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;
		FD_SET(player.clientSocket, &readSet);

		if (player.sendBuffer.Size() > 0)
		{
			FD_SET(player.clientSocket, &sendSet);
		}
	}

	selectRet = select(0, &readSet, &sendSet, NULL, &s_Time);

	if (selectRet > 0)
	{
		if (FD_ISSET(listenSocket, &readSet))
		{
			AcceptProc();
			--selectRet;
		}

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			if (selectRet == 0)
			{
				break;
			}

			Player& player = it->second;

			if (FD_ISSET(player.clientSocket, &readSet))
			{
				RecvProc(it->first);
				--selectRet;
			}

			if (FD_ISSET(player.clientSocket, &sendSet))
			{
				SendProc(it->first);
				--selectRet;
			}
		}
	}
	else if (selectRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SELECT_FAILED;
	}

	DisconnectPlayers();

	return;

SELECT_FAILED:
	g_bShutdown = true;
	return;
}

void AcceptProc(void)
{
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int setSockOptRet;
	linger l;

	Player newPlayer;
	PacketHeader pCMCHeader;
	PacketHeader pCOCHeader;
	PacketSCCreateMyCharacter pCMC;
	PacketSCCreateOtherCharacter pCOC;

#pragma region ACCEPT_CLIENT
	/*
		Accept Client
	*/
	clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		wprintf(L"Create Client Socket Failed");
		goto ACCEPT_FAILED;
	}

	wprintf(L"# [Connected] Client %d.%d.%d.%d:%d\n", clientAddr.sin_addr.S_un.S_un_b.s_b1
												, clientAddr.sin_addr.S_un.S_un_b.s_b2
												, clientAddr.sin_addr.S_un.S_un_b.s_b3
												, clientAddr.sin_addr.S_un.S_un_b.s_b4
												, ntohs(clientAddr.sin_port)
	);

	l.l_onoff = 1;
	l.l_linger = 0;
	setSockOptRet = setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (setSockOptRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SET_SOCK_OPT_FAILED;
	}
#pragma endregion

#pragma region NEW_DATA_INITIALIZE
	/*
		new player, packet initialize
	*/

	newPlayer.status = ePlayerStatus::IDLE;
	newPlayer.clientSocket = clientSocket;
	newPlayer.id = s_CurId;
	newPlayer.x = (rand() % (RANGE_MOVE_RIGHT - RANGE_MOVE_LEFT)) + RANGE_MOVE_LEFT;
	newPlayer.y = (rand() % (RANGE_MOVE_BOTTOM - RANGE_MOVE_TOP)) + RANGE_MOVE_TOP;
	newPlayer.dir = (BYTE)(rand() % 2 * 4); // value is 0(LL) or 4(RR);
	newPlayer.hp = 100;
	s_CurId++;

	CreatePacketCreateMyCharacter(&pCMCHeader, &pCMC, newPlayer.id, newPlayer.dir, newPlayer.x, newPlayer.y, newPlayer.hp);
	CreatePacketCreateOtherCharacter(&pCOCHeader, &pCOC, newPlayer.id, newPlayer.dir, newPlayer.x, newPlayer.y, newPlayer.hp);

#pragma endregion

	g_PlayerList.insert(std::make_pair(newPlayer.id, newPlayer));

#pragma region SEND_DATA_NEW_PLAYER
	SendUnicast(newPlayer.id, sizeof(pCMCHeader), (char*)&pCMCHeader);
	SendUnicast(newPlayer.id, sizeof(pCMC), (char*)&pCMC);
	
	SendBroadcast(&newPlayer.id, 1, sizeof(pCOCHeader), (char*)&pCOCHeader);
	SendBroadcast(&newPlayer.id, 1, sizeof(pCOC), (char*)&pCOC);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->first == newPlayer.id)
		{
			continue;
		}

		CreatePacketCreateOtherCharacter(&pCOCHeader, &pCOC, it->first, it->second.dir, it->second.x, it->second.y, it->second.hp);
		SendUnicast(newPlayer.id, sizeof(pCOCHeader), (char*)&pCOCHeader);
		SendUnicast(newPlayer.id, sizeof(pCOC), (char*)&pCOC);
	}

	wprintf(L"# Create New Client Success\n");
#pragma endregion

	return;

SET_SOCK_OPT_FAILED:
	closesocket(clientSocket);
ACCEPT_FAILED:
	return;
}

void RecvProc(const PlayerID playerId)
{
	Player* recvPlayer;

	char recvBuf[512];
	int recvRet;

	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return;
	}

	recvPlayer = &it->second;

	if (recvPlayer->status == ePlayerStatus::DEAD)
	{
		return;
	}

	// Run Until Wouldblock Error Occurred in ClientSocket
	while (true)
	{
		recvRet = recv(recvPlayer->clientSocket, recvBuf, 512, 0);
		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				break;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client(%d)\n", playerId);
				goto RECV_FAILED;
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto RECV_FAILED;
			}
		}
		else if (recvRet == 0)
		{
			goto RECV_FAILED;
		}

		enqueueSize = recvPlayer->recvBuffer.Enqueue(recvBuf, recvRet);
		if (recvRet != enqueueSize || enqueueSize == 0)
		{
			wprintf(L"Client(%d) RecvBuffer Enqueue Failed\n", playerId);
			goto ENQUEUE_FAILED;
		}

		wprintf(L"Received %d Bytes from Client(%d)\n", recvRet, playerId);
	}

	return;

RECV_FAILED:
ENQUEUE_FAILED:
	DeleteUser(playerId);
	return;
}

void SendProc(const PlayerID playerId)
{
	Player* sendPlayer;

	char sendBuf[512];
	int sendRet;

	size_t peekSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return;
	}

	sendPlayer = &it->second;
	
	if (sendPlayer->status == ePlayerStatus::DEAD)
	{
		return;
	}

	while (sendPlayer->sendBuffer.Size() > 0)
	{
		peekSize = sendPlayer->sendBuffer.Peek(sendBuf, 512);

		sendRet = send(sendPlayer->clientSocket, sendBuf, (int)peekSize, 0);
		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				return;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client(%d)", playerId);
				goto SEND_FAILED;
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto SEND_FAILED;
			}
		}
		
		sendPlayer->sendBuffer.Dequeue(sendRet);
		//sendPlayer->sendBuffer.Dequeue(peekSize);
	}

	return;

SEND_FAILED:
	DeleteUser(playerId);
	return;
}

void SendUnicast(const PlayerID playerId, const size_t size, char* msg)
{
	Player* player;
	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return;
	}

	player = &it->second;

	if (player->status == ePlayerStatus::DEAD)
	{
		return;
	}

	enqueueSize = player->sendBuffer.Enqueue(msg, size);

	if (enqueueSize == 0)
	{
		DeleteUser(playerId);
	}

	return;
}

void SendBroadcast(PlayerID* excludedPlayerId, const size_t playerCount, const size_t size, char* msg)
{
	size_t enqueueSize;
	
	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		for (size_t iCnt = 0; iCnt < playerCount; iCnt++)
		{
			if (it->first == excludedPlayerId[iCnt])
			{
				goto NEXT_LOOP;
			}
		}

		if (it->second.status == ePlayerStatus::DEAD)
		{
			goto NEXT_LOOP;
		}

		enqueueSize = it->second.sendBuffer.Enqueue(msg, size);

		if (enqueueSize == 0)
		{
			DeleteUser(it->first);
		}

	NEXT_LOOP:
		continue;
	}

	return;
}

void DeleteUser(const PlayerID playerId)
{
	auto result = g_PlayerList.find(playerId);

	if (result != g_PlayerList.end())
	{
		result->second.status = ePlayerStatus::DEAD;
		s_CurDead++;
	}

	return;
}

void DisconnectPlayers(void)
{
	PacketHeader pDCHeader;
	PacketSCDeleteCharacter pDC;

	if (s_CurDead == 0)
	{
		return;
	}

	wprintf(L"Disconnect %zu Players\n", s_CurDead);

	pDCHeader.code = 0x89;
	pDCHeader.size = sizeof(pDC);
	pDCHeader.type = (BYTE)ePacketType::PACKET_SC_DELETE_CHARACTER;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->second.status != ePlayerStatus::DEAD)
		{
			continue;
		}

		closesocket(it->second.clientSocket);
		pDC.id = it->first;

		SendBroadcast(NULL, 0, sizeof(pDCHeader), (char*)&pDCHeader);
		SendBroadcast(NULL, 0, sizeof(pDC), (char*)&pDC);

		wprintf(L"Disconnect : Client(%d)\n", it->first);

		it = g_PlayerList.erase(it);
	}

	wprintf(L"Disconnect Success\n");
	s_CurDead = 0;
}

void Terminate(void)
{
	closesocket(listenSocket);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		closesocket(it->second.clientSocket);
	}

	WSACleanup();

	return;
}