#pragma once

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#define _WINSOCKAPI_

#include <unordered_map>
#include "Player.h"

extern std::unordered_map<PlayerID_t, Player> g_PlayerList;

extern SOCKET listenSocket;
extern SOCKADDR_IN serverAddr;

int SocketInitialize(void); // return true if success.
void MessageController(void);
void AcceptProc(void); // New User Connected
void RecvProc(PlayerID_t* player); // Player*;
void SendUnicast(PlayerID_t player, char* msg); // send msg to player
void SendBroadcast(PlayerID_t* excludedPlayer, size_t playerCnt, char* msg); // send msg to all player excluding player
void Disconnect(PlayerID_t player);