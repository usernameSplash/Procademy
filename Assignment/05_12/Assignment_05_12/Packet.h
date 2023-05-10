#pragma once

/////////////////////////////////////////////////////////////////////
// www.gamecodi.com				origin : ¿Ã¡÷«‡ master@gamecodi.com
//								edit   : ±Ë∞ÊπŒ kkfenix3085@gmail.com
//
/////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include "Player.h"

enum class ePacketType
{
	PACKET_SC_CREATE_MY_CHARACTER = 0,
	PACKET_SC_CREATE_OTHER_CHARACTER,
	PACKET_SC_DELETE_CHARACTER,

	PACKET_CS_MOVE_START = 10,
	PACKET_SC_MOVE_START,
	PACKET_CS_MOVE_STOP,
	PACKET_SC_MOVE_STOP,

	PACKET_CS_ATTACK_1 = 20,
	PACKET_SC_ATTACK_1,
	PACKET_CS_ATTACK_2,
	PACKET_SC_ATTACK_2,
	PACKET_CS_ATTACK_3,
	PACKET_SC_ATTACK_3,

	PACKET_SC_DAMAGE = 30,

	PACKET_CS_SYNC = 250,
	PACKET_SC_SYNC,
};

#pragma pack(push, 1)
struct PacketHeader
{
	BYTE code;
	BYTE size;
	BYTE type;
};

struct PacketSCCreateMyCharacter
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
	BYTE hp;
};

struct PacketSCCreateOtherCharacter
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
	BYTE hp;
};

struct PacketSCDeleteCharacter
{
	PlayerID id;
};

struct PacketCSMoveStart
{
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCMoveStart
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketCSMoveStop
{
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCMoveStop
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketCSAttack1
{
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCAttack1
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketCSAttack2
{
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCAttack2
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketCSAttack3
{
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCAttack3
{
	PlayerID id;
	BYTE dir;
	SHORT x;
	SHORT y;
};

struct PacketSCDamage
{
	PlayerID attackId;
	PlayerID damagedId;
	BYTE damagedHp;
};

struct PacketCSSync
{
	SHORT x;
	SHORT y;
};

struct PacketSCSync
{
	PlayerID id;
	SHORT x;
	SHORT y;
};

#pragma pack(pop)
