
#include "Network.h"
#include "Packet.h"

std::unordered_map<PlayerID, Player> g_ShouldDisconnectPlayerList;
std::unordered_map<PlayerID, Player> g_PlayerList;

SOCKET listenSocket;
SOCKADDR_IN serverAddr;
fd_set readSet;
fd_set sendSet;

static timeval time { 0,0 };

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

	selectRet = select(0, &readSet, NULL, NULL, &time);
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

SELECT_FAILED:
	return;
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

	selectRet = select(0, NULL, &sendSet, NULL, &time);
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
	// Accept Tasks
	return;
}

BOOL RecvProc(const PlayerID& playerId)
{
	// Receive Data from Specific Player
	return TRUE;
}

BOOL SendProc(const PlayerID& playerId)
{
	// Send Data to Specific Player
	return TRUE;
}

void SendUnicast(const PlayerID& playerId, const size_t size, char* msg)
{

}

void SendBroadcast(PlayerID* excludedPlayerId, const size_t playerCount, const size_t size, char* msg)
{

}

std::unordered_map<PlayerID, Player>::iterator DeleteUser(const PlayerID& playerId)
{
	return g_PlayerList.end();
}

void Disconnect(const PlayerID& playerId)
{

}

void DisconnectPlayers(void)
{

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