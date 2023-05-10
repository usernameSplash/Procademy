#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <unordered_map>

#include "Player.h"

extern std::unordered_map<PlayerID, Player> g_ShouldDisconnectPlayerList;
extern std::unordered_map<PlayerID, Player> g_PlayerList;

extern SOCKET listenSocket;
extern SOCKADDR_IN serverAddr;
extern fd_set readSet;
extern fd_set sendSet;

BOOL SocketInitialize(void);


/*
* Below Five Functions return FALSE if socket error occurred
*/
BOOL RecvMessageController(void);
BOOL SendMessageController(void); 
void AcceptProc(void);
BOOL RecvProc(const PlayerID& playerId);
BOOL SendProc(const PlayerID& playerId);

void SendUnicast(const PlayerID& playerId, const size_t size, char* msg);
void SendBroadcast(PlayerID* excludedPlayerId, const size_t playerCount, const size_t size, char* msg);

std::unordered_map<PlayerID, Player>::iterator DeleteUser(const PlayerID& playerId);
void Disconnect(const PlayerID& playerId);
void DisconnectPlayers(void);

void Terminate(void);