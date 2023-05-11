
#include <cstdlib>

#include "Network.h"
#include "Packet.h"

std::unordered_map<PlayerID, Player> g_ShouldDisconnectPlayerList;
std::unordered_map<PlayerID, Player> g_PlayerList;

SOCKET listenSocket;
SOCKADDR_IN serverAddr;
fd_set readSet;
fd_set sendSet;

static INT32 s_CurId = 0;

static timeval s_Time { 0,0 };

#define SERVER_PORT 5000

BOOL SocketInitialize(void)
{
	g_ShouldDisconnectPlayerList.reserve(20);
	g_PlayerList.reserve(20);

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

BOOL RecvMessageController(void)
{
	int selectRet;

	FD_SET(listenSocket, &readSet);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;
		FD_SET(player.clientSocket, &readSet);
	}

	selectRet = select(0, &readSet, NULL, NULL, &s_Time);
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

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;

		if (FD_ISSET(player.clientSocket, &sendSet))
		{
			BOOL result = RecvProc(it->first);
			if (result == FALSE)
			{
				// TODO (disconnect current player)
			}
		}
	}

	return TRUE;

SELECT_FAILED:
	return FALSE;
}

BOOL SendMessageController(void)
{
	int selectRet;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;

		if (player.sendBuffer.Size() > 0)
		{
			FD_SET(player.clientSocket, &sendSet);
		}
	}

	selectRet = select(0, NULL, &sendSet, NULL, &s_Time);
	if (selectRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		goto SELECT_FAILED;
	}

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;

		if (FD_ISSET(player.clientSocket, &sendSet))
		{
			BOOL result = SendProc(it->first);
			if (result == FALSE)
			{
				// TODO (disconnect current player)
			}
		}
	}
	
	return TRUE;

SELECT_FAILED:
	return FALSE;
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

	wprintf(L"Client %d.%d.%d.%d Connected\n", clientAddr.sin_addr.S_un.S_un_b.s_b1
											 , clientAddr.sin_addr.S_un.S_un_b.s_b2
											 , clientAddr.sin_addr.S_un.S_un_b.s_b3
											 , clientAddr.sin_addr.S_un.S_un_b.s_b4
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

	newPlayer.clientSocket = clientSocket;
	newPlayer.id = s_CurId;
	newPlayer.bMoving = FALSE;
	newPlayer.x = (rand() % (RANGE_MOVE_RIGHT - RANGE_MOVE_LEFT)) + RANGE_MOVE_LEFT;
	newPlayer.y = (rand() % (RANGE_MOVE_BOTTOM - RANGE_MOVE_TOP)) + RANGE_MOVE_TOP;
	newPlayer.dir = (BYTE)(rand() % 2 * 4); // value is 0(LL) or 4(RR);
	newPlayer.hp = 100;
	s_CurId++;

	pCMCHeader.code = 0x89;
	pCMCHeader.size = sizeof(pCMC);
	pCMCHeader.type = (BYTE)ePacketType::PACKET_SC_CREATE_MY_CHARACTER;

	pCMC.id = newPlayer.id;
	pCMC.dir = newPlayer.dir;
	pCMC.x = newPlayer.x;
	pCMC.y = newPlayer.y;
	pCMC.hp = newPlayer.hp;

	pCOCHeader.code = 0x89;
	pCOCHeader.size = sizeof(pCOC);
	pCOCHeader.type = (BYTE)ePacketType::PACKET_SC_CREATE_OTHER_CHARACTER;

	pCOC.id = newPlayer.id;
	pCOC.dir = newPlayer.dir;
	pCOC.x = newPlayer.x;
	pCOC.y = newPlayer.y;
	pCOC.hp = newPlayer.hp;
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

		pCOC.id = it->first;
		pCOC.dir = it->second.dir;
		pCOC.x = it->second.x;
		pCOC.y = it->second.y;
		pCOC.hp = it->second.hp;

		SendUnicast(newPlayer.id, sizeof(pCOC), (char*)&pCOC);
	}
	wprintf(L"Create New Client Success\n");
#pragma endregion

	return;

SET_SOCK_OPT_FAILED:
	closesocket(clientSocket);
ACCEPT_FAILED:
	return;
}

BOOL RecvProc(const PlayerID& playerId)
{
	Player* recvPlayer;

	char recvBuf[512];
	int recvRet;

	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return TRUE;
	}

	recvPlayer = &it->second;

	// Run Until Wouldblock Error Occurred in ClientSocket
	while (TRUE)
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
				wprintf(L"Disconnected By Client(%d)", playerId);
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
			goto ENQUEUE_FAILED;
		}

		wprintf(L"Received %d Bytes from Client(%d)\n", recvRet, playerId);
	}

	return TRUE;

RECV_FAILED:
ENQUEUE_FAILED:
	return FALSE;
}

BOOL SendProc(const PlayerID& playerId)
{
	Player* sendPlayer;

	char sendBuf[512];
	int sendRet;

	size_t peekSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return TRUE;
	}

	sendPlayer = &it->second;
	
	while (sendPlayer->sendBuffer.Size() > 0)
	{
		peekSize = sendPlayer->sendBuffer.Peek(sendBuf, 512);

		sendRet = send(sendPlayer->clientSocket, sendBuf, (int)peekSize, 0);
		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				return TRUE;
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
	return TRUE;

SEND_FAILED:
	return FALSE;
}

void SendUnicast(const PlayerID& playerId, const size_t size, char* msg)
{
	Player* player;
	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return;
	}

	player = &it->second;
	enqueueSize = player->sendBuffer.Enqueue(msg, size);

	if (enqueueSize == 0)
	{
		DeleteUser(playerId);
	}
}

void SendBroadcast(PlayerID* excludedPlayerId, const size_t playerCount, const size_t size, char* msg)
{
	size_t enqueueSize;

	for (size_t iCnt = 0; iCnt < playerCount; iCnt++)
	{
		auto it = g_PlayerList.find(excludedPlayerId[iCnt]);
		if (it == g_PlayerList.end())
		{
			return;
		}
	}
	
	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		for (size_t iCnt = 0; iCnt < playerCount; iCnt++)
		{
			auto it = g_PlayerList.find(excludedPlayerId[iCnt]);
			if (it == g_PlayerList.end())
			{
				return;
			}
		}

		enqueueSize = it->second.sendBuffer.Enqueue(msg, size);
		if (enqueueSize == 0)
		{
			auto deleteElem = DeleteUser(it->first);
			
			if (deleteElem == g_PlayerList.end() && !g_PlayerList.empty())
			{
				continue;
			}

			it = deleteElem;
		}
	}
}

std::unordered_map<PlayerID, Player>::iterator DeleteUser(const PlayerID& playerId)
{
	std::unordered_map<PlayerID, Player>::iterator result;

	result = g_PlayerList.find(playerId);

	if (result != g_PlayerList.end())
	{
		g_ShouldDisconnectPlayerList.insert(std::make_pair(playerId, result->second));
		result = g_PlayerList.erase(result);
	}

	return result;
}

void DisconnectPlayers(void)
{
	PacketHeader pDCHeader;
	PacketSCDeleteCharacter pDC;

	if (!g_ShouldDisconnectPlayerList.empty())
	{
		wprintf(L"Disconnect %zu Players\n", g_ShouldDisconnectPlayerList.size());
	}

	pDCHeader.code = 0x89;
	pDCHeader.size = sizeof(pDC);
	pDCHeader.type = (BYTE)ePacketType::PACKET_SC_DELETE_CHARACTER;

	for (auto it = g_ShouldDisconnectPlayerList.begin(); it != g_ShouldDisconnectPlayerList.end(); ++it)
	{
		closesocket(it->second.clientSocket);
		pDC.id = it->first;

		SendBroadcast(NULL, 0, sizeof(pDCHeader), (char*)&pDCHeader);
		SendBroadcast(NULL, 0, sizeof(pDC), (char*)&pDC);

		wprintf(L"Disconnect : Client(%d)\n", it->first);
	}

	g_ShouldDisconnectPlayerList.clear();
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