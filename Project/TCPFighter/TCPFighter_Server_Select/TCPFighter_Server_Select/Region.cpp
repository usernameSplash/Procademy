#include "Region.h"

namespace TCPFighter_Select_Server
{
	void Region::Initialize(short x, short y)
	{
		_x = x;
		_y = y;

		if ((x >= 2 && x < (REGION_MAX_X - 2)) && (y >= 2 && y < (REGION_MAX_Y - 2)))
		{
			_xMin = (x - 2) * REGION_SIZE_X;
			_yMin = (y - 2) * REGION_SIZE_Y;
			_xMax = _xMin + REGION_SIZE_X;
			_yMax = _yMin + REGION_SIZE_Y;

			_playerList.reserve(MAX_PLAYER_PER_REGION);
		}
	}
}