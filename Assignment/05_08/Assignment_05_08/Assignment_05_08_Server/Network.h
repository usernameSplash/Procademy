#pragma once

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#define _WINSOCKAPI_

#include <unordered_map>
#include "Player.h"

extern std::unordered_map<PlayerID_t, Player> g_ShouldDisconnectPlayerList;
extern std::unordered_map<PlayerID_t, Player> g_PlayerList;

extern SOCKET listenSocket;
extern SOCKADDR_IN serverAddr;
extern fd_set readSet;
extern fd_set sendSet;

extern PlayerID_t g_PlayerId;

int SocketInitialize(void); // return true if success.

void MessageController(void);

void AcceptProc(void); // New User Connected
int RecvProc(const PlayerID_t& playerId); // return 1 if receive failed(should delete user)
void LogicProc(void);	// Logic(Processing Received Data of each user)
int SendProc(const PlayerID_t& playerId); // return 1 if send failed(should delete user)

void SendUnicast(const PlayerID_t& playerId, const size_t size, char* msg); // send msg to player
void SendBroadcast(PlayerID_t* excludedPlayerId, const size_t playerCount, const size_t size, char* msg); // send msg to all player excluding player

std::unordered_map<PlayerID_t, Player>::iterator DeleteUser(const PlayerID_t& playerId);	// delete user in <g_PlayerList>, return next iterator
void Disconnect(const PlayerID_t& playerId);	// disconnect network from a user
void DisconnectPlayers(void);	// call Disconnect function each element in <g_ShouldDisconnectPlayerList> 