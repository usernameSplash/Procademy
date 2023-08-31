#pragma once

#include "Session.h"
#include "Region.h"

//-----------------------------------------------------------------
// ȭ�� �̵� ����.
//-----------------------------------------------------------------
#define RANGE_MOVE_TOP	0
#define RANGE_MOVE_LEFT	0
#define RANGE_MOVE_RIGHT	6400
#define RANGE_MOVE_BOTTOM	6400

//---------------------------------------------------------------
// ���ݹ���.
//---------------------------------------------------------------
#define ATTACK1_RANGE_X		80
#define ATTACK2_RANGE_X		90
#define ATTACK3_RANGE_X		100
#define ATTACK1_RANGE_Y		10
#define ATTACK2_RANGE_Y		10
#define ATTACK3_RANGE_Y		20

//---------------------------------------------------------------
// ���� ������.
//---------------------------------------------------------------
#define ATTACK1_DAMAGE		1
#define ATTACK2_DAMAGE		2
#define ATTACK3_DAMAGE		3

//-----------------------------------------------------------------
// ĳ���� �̵� �ӵ�   // 25fps ���� �̵��ӵ�
//-----------------------------------------------------------------
#define SPEED_PLAYER_X	6	// 3   50fps
#define SPEED_PLAYER_Y	4	// 2   50fps

#define MAX_PLAYER_HP 100

namespace TCPFighter_Select_Server
{
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
		IDLE,
		MOVE,
		ATTACK,
		DEAD,
		INVALID
	};

	struct Region;
	struct Player
	{
	public:
		Player() = default;

	public:
		void Initialize(int id, Session* session);

	public:
		int _id;
		eMoveDir _dir;
		eMoveDir _moveDir;
		ePlayerStatus _status;
		char _hp;
		short _x;
		short _y;
		Session* _session;
		Region* _region;
	};
}