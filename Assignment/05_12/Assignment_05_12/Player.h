#pragma once

#include <WinSock2.h>
#include "RingBuffer.h"

constexpr int PLAYER_SPEED_X = 3;
constexpr int PLAYER_SPEED_Y = 2;

#define RANGE_MOVE_TOP		50
#define RANGE_MOVE_LEFT		10
#define RANGE_MOVE_RIGHT	630
#define RANGE_MOVE_BOTTOM	470

#define RANGE_MOVE_ERROR	50

#define RANGE_ATTACK_1_X	80
#define RANGE_ATTACK_2_X	90
#define RANGE_ATTACK_3_X	100
#define RANGE_ATTACK_1_Y	10
#define RANGE_ATTACK_2_Y	10
#define RANGE_ATTACK_3_Y	20

using namespace MyDataStructure;

typedef __int32 PlayerID;

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

enum class ePlayerStatus
{
	IDLE = 0,
	MOVE,
	ATTACK,
	DEAD,
	INVALID
};

struct Player
{
	ePlayerStatus status {};
	SOCKET clientSocket {};
	PlayerID id {};
	RingBuffer recvBuffer {};
	RingBuffer sendBuffer {};
	SHORT x {};
	SHORT y {};
	BYTE dir {};
	BYTE hp {};
};
