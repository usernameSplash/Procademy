#pragma once
#include <basetsd.h>
#include <WinSock2.h>

typedef UINT32 PlayerID_t;

typedef struct Player
{
	PlayerID_t id;
	SOCKET clientSocket;
	INT32 x;
	INT32 y;
} Player_t;