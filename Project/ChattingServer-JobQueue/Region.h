#pragma once

#include <vector>

using namespace std;

class Player;

class Region
{
public:
	Region()
	{
		_aroundRegion.reserve(8);
		_players.reserve(5000);
	}
public:
	vector<Region*> _aroundRegion;
	vector<Player*> _players;
	
	short _x;
	short _y;
};