#pragma once

#include <vector>
#include "Player.h"

using namespace std;

#define REGION_SIZE_X 200
#define REGION_SIZE_Y 200
#define REGION_MAX_X ((RANGE_MOVE_RIGHT / REGION_SIZE_X) + 4) // Real Regions and 4 Dummy Regions
#define REGION_MAX_Y ((RANGE_MOVE_BOTTOM / REGION_SIZE_Y) + 4)
#define REGION_NUM_DIAG 5
#define REGION_NUM_ORTHO 3
#define MAX_PLAYER_PER_REGION 512

namespace TCPFighter_Select_Server
{
	struct Player;
	struct Region
	{
	public:
		void Initialize(const short x, const short y);

	public:
		short _x;
		short _y;
		short _xMin;
		short _yMin;
		short _xMax;
		short _yMax;
		vector<Player*> _playerList;

	public:
		Region* _llNew[REGION_NUM_ORTHO];
		Region* _luNew[REGION_NUM_DIAG];
		Region* _uuNew[REGION_NUM_ORTHO];
		Region* _ruNew[REGION_NUM_DIAG];
		Region* _rrNew[REGION_NUM_ORTHO];
		Region* _rdNew[REGION_NUM_DIAG];
		Region* _ddNew[REGION_NUM_ORTHO];
		Region* _ldNew[REGION_NUM_DIAG];

		Region* _llOld[REGION_NUM_ORTHO];
		Region* _luOld[REGION_NUM_DIAG];
		Region* _uuOld[REGION_NUM_ORTHO];
		Region* _ruOld[REGION_NUM_DIAG];
		Region* _rrOld[REGION_NUM_ORTHO];
		Region* _rdOld[REGION_NUM_DIAG];
		Region* _ddOld[REGION_NUM_ORTHO];
		Region* _ldOld[REGION_NUM_DIAG];

	public:
		Region* _aroundRegions[9];

		Region** _newRegions[8] = {
			_llNew, _luNew, _uuNew, _ruNew, _rrNew, _rdNew, _ddNew, _ldNew
		};

		Region** _oldRegions[8] = {
			_llOld, _luOld, _uuOld, _ruOld, _rrOld, _rdOld, _ddOld, _ldOld
		};
	};
}