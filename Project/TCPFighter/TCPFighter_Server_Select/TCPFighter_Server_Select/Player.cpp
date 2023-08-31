#include "Player.h"
#include <stdlib.h>

namespace TCPFighter_Select_Server
{
	void Player::Initialize(int id, Session* session)
	{
		_id = id;

		_dir = eMoveDir::LL;
		_moveDir = eMoveDir::LL;
		_status = ePlayerStatus::IDLE;
		_hp = MAX_PLAYER_HP;

		_session = session;
		_region = nullptr;

		_x = rand() % RANGE_MOVE_RIGHT;
		_y = rand() % RANGE_MOVE_BOTTOM;
	}
}