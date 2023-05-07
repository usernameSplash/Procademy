#pragma once
#include <basetsd.h>

enum class ePacketType
{
	ASSIGN_ID = 0,
	CREATE_STAR,
	DELETE_STAR,
	MOVE_STAR
};

typedef struct PacketAssignID
{
	ePacketType type;
	INT32 id;
	INT32 temp[2];
} PacketAssignID_t;

typedef struct PacketCreateStar
{
	ePacketType type;
	INT32 id;
	INT32 xCoord;
	INT32 yCoord;
} PacketCreateStar_t;

typedef struct PacketDeleteStar
{
	ePacketType type;
	INT32 id;
	INT32 temp[2];
} PacketDeleteStar_t;

typedef struct PacketMoveStar
{
	ePacketType type;
	INT32 id;
	INT32 xCoord;
	INT32 yCoord;
} PacketMoveStar_t;