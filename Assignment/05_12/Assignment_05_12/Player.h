#pragma once

#include <WinSock2.h>
#include "RingBuffer.h"

constexpr int PLAYER_SPEED_X = 3;
constexpr int PLAYER_SPEED_Y = 2;

#define RANGE_MOVE_TOP		50
#define RANGE_MOVE_LEFT		10
#define RANGE_MOVE_RIGHT	630
#define RANGE_MOVE_BOTTOM	470

using namespace MyDataStructure;

typedef INT32 PlayerID;

enum class eMoveDir
{
	LL = 0,
	LU,
	UU,
	RU,
	RR,
	RD,
	DD,
	LD,
};

struct Player
{
	SOCKET clientSocket;
	INT32 id;
	RingBuffer recvBuffer;
	RingBuffer sendBuffer;
	BOOL bMoving;
	SHORT x;
	SHORT y;
	BYTE dir;
	BYTE hp;
};
