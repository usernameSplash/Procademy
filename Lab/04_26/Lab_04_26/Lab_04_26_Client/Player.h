#pragma once
#include <basetsd.h>

typedef UINT32 PlayerID_t;

typedef struct Player
{
	PlayerID_t id;
	INT32 x;
	INT32 y;
} Player_t;