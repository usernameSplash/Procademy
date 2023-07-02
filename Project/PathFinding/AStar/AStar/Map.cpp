#include "Map.h"

namespace PathFinder
{
	Map::Map(const size_t width, const size_t depth)
		: _width(width)
		, _depth(depth)
		, _startNode(nullptr)
		, _destNode(nullptr)
	{
		_grid.reserve(_width * _depth);
		_grid.resize(_width * _depth, false);
	}

	void Map::SetValue(const size_t x, const size_t y, bool value)
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return;
		}

		_grid[_width * y + x] = value;
	}

	bool Map::GetValue(const size_t x, const size_t y) const
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return;
		}

		return _grid[_width * y + x];
	}
}