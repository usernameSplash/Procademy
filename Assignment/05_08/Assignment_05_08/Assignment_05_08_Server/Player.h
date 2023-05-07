#pragma once
#include <basetsd.h>
#include <WinSock2.h>

#include "RingBuffer.h"

typedef UINT32 PlayerID_t;

using namespace MyDataStructure;

typedef struct Player
{
	PlayerID_t id {};
	SOCKET clientSocket {};
	RingBuffer recvBuf;
	RingBuffer sendBuf;
	INT32 x {};
	INT32 y {};
} Player_t;