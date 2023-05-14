#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <unordered_map>

#include "Player.h"

extern bool g_bShutdown;

extern std::unordered_map<PlayerID, Player> g_PlayerList;

extern SOCKET listenSocket;
extern SOCKADDR_IN serverAddr;
extern fd_set readSet;
extern fd_set sendSet;

bool SocketInitialize(void);

void MessageController(void);
void AcceptProc(void);
void RecvProc(const PlayerID playerId);
void SendProc(const PlayerID playerId);

void SendUnicast(const PlayerID playerId, const size_t size, char* msg);
void SendBroadcast(PlayerID* excludedPlayerId, const size_t playerCount, const size_t size, char* msg);

void DeleteUser(const PlayerID playerId);
void DisconnectPlayers(void);

void Terminate(void);