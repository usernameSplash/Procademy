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

PlayerID_t g_PlayerId = 0;

#define SERVER_PORT 3000

using namespace MyDataStructure;

int SocketInitialize(void)
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
	timeval time;

	time.tv_sec = 0;
	time.tv_usec = 0;

	while (TRUE)
	{
		FD_ZERO(&readSet);
		FD_ZERO(&sendSet);

#pragma region SELECT_RECV
		FD_SET(listenSocket, &readSet);

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			FD_SET(it->second.clientSocket, &readSet);
		}

		selectRet = select(0, &readSet, NULL, NULL, &time);

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

		if (readSet.fd_count > 0)
		{
			wprintf(L"\nReceive Data From %u Users\n", readSet.fd_count);
		}

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			if (FD_ISSET(it->second.clientSocket, &readSet))
			{
				wprintf(L"Receive Data From Client\n");
				int result = RecvProc(it->first);
				if (result == 1)
				{
					auto deleteElem = DeleteUser(it->first);
					if (deleteElem == g_PlayerList.end() && (!g_PlayerList.empty()))
					{
						continue;
					}

					it = deleteElem;
				}
			}
		}

#pragma endregion

		LogicProc();

#pragma region SELECT_SEND

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			if (it->second.sendBuf.Size() > 0)
			{
				FD_SET(it->second.clientSocket, &sendSet);
			}
		}

		if (sendSet.fd_count > 0)
		{
			selectRet = select(0, NULL, &sendSet, NULL, &time);

			if (selectRet == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto SELECT_FAILED;
			}

			wprintf(L"\n%u Users send Data\n", sendSet.fd_count);
			for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
			{
				if (FD_ISSET(it->second.clientSocket, &sendSet))
				{
					wprintf(L"Send Data To Client\n");
					int result = SendProc(it->first);
					if (result == 1)
					{
						auto deleteElem = DeleteUser(it->first);
						if (deleteElem == g_PlayerList.end() && (!g_PlayerList.empty()))
						{
							continue;
						}

						it = deleteElem;
					}
				}
			}
		}

#pragma endregion
		DisconnectPlayers();
	}

SELECT_FAILED:
	closesocket(listenSocket);
	WSACleanup();
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
	PacketAssignID_t packetAssignID;
	PacketCreateStar_t packetCreateStar;

	clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
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

	newPlayer.clientSocket = clientSocket;
	newPlayer.id = g_PlayerId;
	newPlayer.x = 40;
	newPlayer.y = 12;

	g_PlayerId++;

	packetAssignID.type = ePacketType::ASSIGN_ID;
	packetAssignID.id = newPlayer.id;

	packetCreateStar.type = ePacketType::CREATE_STAR;
	packetCreateStar.id = newPlayer.id;
	packetCreateStar.xCoord = newPlayer.x;
	packetCreateStar.yCoord = newPlayer.y;

	g_PlayerList.insert(std::make_pair(newPlayer.id, newPlayer));

	SendUnicast(newPlayer.id, 16, (char*)&packetAssignID);

	SendUnicast(newPlayer.id, 16, (char*)&packetCreateStar);
	SendBroadcast(&newPlayer.id, 1, 16, (char*)&packetCreateStar);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->first == newPlayer.id)
		{
			continue;
		}

		packetCreateStar.id = it->first;
		packetCreateStar.xCoord = it->second.x;
		packetCreateStar.yCoord = it->second.y;
		SendUnicast(newPlayer.id, 16, (char*)&packetCreateStar);
	}

	wprintf(L"Create New Client Success\n");

LINGER_SET_FAILED:
ACCEPT_FAILED:
	return;
}

int RecvProc(const PlayerID_t& playerId)
{
	Player* recvPlayer;

	UINT8 recvBuf[16 * 32];
	int recvRet;

	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return 0;
	}

	recvPlayer = &it->second;

	while (TRUE)
	{
		recvRet = recv(recvPlayer->clientSocket, (char*)recvBuf, 16 * 32, 0);

		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				wprintf(L"WOULDBLOCK\n");
				return 0;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client\n");
				goto RECV_FAILED;
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto RECV_FAILED;
			}
		}
		else if(recvRet == 0)
		{
			goto RECV_FAILED;
		}

		wprintf(L"Received %d Bytes from Client(%u)\n", recvRet, playerId);
		enqueueSize = recvPlayer->recvBuf.Enqueue((char*)recvBuf, recvRet);

		if (recvRet != enqueueSize || enqueueSize == 0)
		{
			goto ENQUEUE_FAILED;
		}
	}
	return 0;

RECV_FAILED:
ENQUEUE_FAILED:
	//DeleteUser(playerId);
	return 1;
}

void LogicProc(void)
{
	ePacketType type;
	size_t peekSize;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player* p = &it->second;
		if (p->recvBuf.Size() == 0)
		{
			continue;
		}

		peekSize = p->recvBuf.Peek((char*)&type, sizeof(type));

		if (peekSize < sizeof(type))
		{
			goto LOOP_BREAK;
		}

		switch (type)
		{
			case ePacketType::MOVE_STAR:
				{
					PacketMoveStar_t packetMoveStar;
					
					peekSize = p->recvBuf.Peek((char*)&packetMoveStar, sizeof(packetMoveStar));
					if (peekSize < sizeof(packetMoveStar))
					{
						goto LOOP_BREAK;
					}
					p->recvBuf.Dequeue(sizeof(packetMoveStar));

					packetMoveStar.type = ePacketType::MOVE_STAR;

					auto it = g_PlayerList.find(packetMoveStar.id);

					if (it == g_PlayerList.end())
					{
						break;
					}

					Player_t* pPlayer = &(it->second);
					pPlayer->x = packetMoveStar.xCoord;
					pPlayer->y = packetMoveStar.yCoord;

					SendBroadcast(&pPlayer->id, 1, 16, (char*)&packetMoveStar);
					break;
				}
		default:
			break;
		}

		continue;
	LOOP_BREAK:
		break;
	}
}

int SendProc(const PlayerID_t& playerId)
{
	Player* sendPlayer;

	UINT8 sendBuf[16 * 32];
	int sendRet;

	size_t peekSize;

	auto it = g_PlayerList.find(playerId);
	if (it == g_PlayerList.end())
	{
		return 0;
	}

	sendPlayer = &it->second;

	while (sendPlayer->sendBuf.Size() > 0)
	{
		peekSize = sendPlayer->sendBuf.Peek((char*)sendBuf, sendPlayer->sendBuf.Size());

		sendRet = send(sendPlayer->clientSocket, (char*)sendBuf, (int)peekSize, 0);
		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				wprintf(L"WOULDBLOCK\n");
				return 0;
			}
			else if (errorCode == WSAECONNRESET)
			{
				wprintf(L"Disconnected By Client\n");
				goto SEND_FAILED;
			}
			else
			{
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				goto SEND_FAILED;
			}
		}

		sendPlayer->sendBuf.Dequeue(peekSize);
	}
	return 0;

SEND_FAILED:
	return 1;
}

void SendUnicast(const PlayerID_t& playerId, const size_t size, char* msg)
{
	Player* player;
	size_t enqueueSize;

	auto it = g_PlayerList.find(playerId);

	if (it == g_PlayerList.end())
	{
		return;
	}

	player = &it->second;
	enqueueSize = player->sendBuf.Enqueue(msg, size);

	if (enqueueSize == 0)
	{
		DeleteUser(it->first);
	}
	//if (sendRet == SOCKET_ERROR)
	//{
	//	int errorCode = WSAGetLastError();
	//	if (errorCode == WSAEWOULDBLOCK)
	//	{
	//		return;
	//	}
	//	else if (errorCode == WSAECONNRESET)
	//	{
	//		wprintf(L"Disconnected By Client(%u) in Broadcasting\n", player.id);
	//	}
	//	else
	//	{
	//		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
	//	}
	//	DeleteUser(playerId);

	//	return;
	//}

	return;
}

void SendBroadcast(PlayerID_t* excludedPlayerId, const size_t playerCount, const size_t size, char* msg)
{
	size_t enqueueSize;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		for (int iCnt = 0; iCnt < playerCount; ++iCnt)
		{
			if (it->first == excludedPlayerId[iCnt])
			{
				goto NEXT_LOOP;
			}
		}

		enqueueSize = it->second.sendBuf.Enqueue(msg, size);
		if (enqueueSize == 0)
		{
			DeleteUser(it->first);
		}
		//if (sendRet == SOCKET_ERROR)
		//{
		//	int errorCode = WSAGetLastError();
		//	if (errorCode == WSAEWOULDBLOCK)
		//	{
		//		break;
		//	}
		//	else if (errorCode == WSAECONNRESET)
		//	{
		//		wprintf(L"Disconnected By Client(%u) in Broadcasting\n", it->second.id);
		//	}
		//	else
		//	{
		//		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		//	}
		//	DeleteUser(const_cast<PlayerID_t&>(it->first));
		//}
		//else
		//{
		//	wprintf(L"[Broadcast Message] Type: %d, Dest ID: %u\n", *reinterpret_cast<ePacketType*>(msg), it->second.id);
		//}

	NEXT_LOOP:
		continue;
	}

	return;
}

std::unordered_map<PlayerID_t, Player>::iterator DeleteUser(const PlayerID_t& playerId)
{
	std::unordered_map<PlayerID_t, Player>::iterator result;

	result = g_PlayerList.find(playerId);

	if (result != g_PlayerList.end())
	{
		g_ShouldDisconnectPlayerList.insert(std::make_pair(playerId, g_PlayerList.find(playerId)->second));
		result = g_PlayerList.erase(result);
	}

	return result;
}

void Disconnect(const PlayerID_t& playerId)
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

	SendBroadcast(NULL, 0, 16, (char*)&packetDeleteStar);

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
