#include "Map.h"

namespace PathFinder
{
	Map::Map(const int width, const int depth)
		: _width(width)
		, _depth(depth)
		, _startPos(0, 0)
		, _destPos(width - 1, depth - 1)
	{
		_grid.resize(_width * _depth, eGridStatus::NORMAL);
		_oldGrid.resize(_width * _depth, eGridStatus::NORMAL);
		_gValueGrid.resize(_width * _depth, -1);

		_grid[0] = eGridStatus::START;
		_grid[width * depth - 1] = eGridStatus::DEST;
	}

	Map::~Map()
	{
	}

	void Map::SetValue(const int x, const int y, const eGridStatus status)
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return;
		}

		switch (status)
		{
		case eGridStatus::NORMAL:
			/* intentional fallthrough */
		case eGridStatus::BLOCKED:
			/* intentional fallthrough */
		case eGridStatus::SEARCHED:
			/* intentional fallthrough */
		case eGridStatus::VISITED:
			{
				if(_grid[y * _width + x] != eGridStatus::START && _grid[y*_width+x] != eGridStatus::DEST)
				{
					_grid[y * _width + x] = status;
				}
				break;
			}
		case eGridStatus::START:
			{
				if (_grid[y * _width + x] != eGridStatus::DEST)
				{
					_grid[_startPos._y * _width + _startPos._x] = eGridStatus::NORMAL;
					_grid[y * _width + x] = status;
					_startPos = { x, y };
				}
				break;
			}
		case eGridStatus::DEST:
			{
				if (_grid[y * _width + x] != eGridStatus::START)
				{
					_grid[_destPos._y * _width + _destPos._x] = eGridStatus::NORMAL;
					_grid[y * _width + x] = status;
					_destPos = { x, y };
				}
				break;
			}
		default:
			break;
		}
	}

	void Map::SetValue(const Vector2 pos, const eGridStatus status)
	{
		if (pos._y >= _depth || pos._x >= _width || pos._y < 0 || pos._x < 0)
		{
			return;
		}

		switch (status)
		{
		case eGridStatus::NORMAL:
			/* intentional fallthrough */
		case eGridStatus::BLOCKED:
			/* intentional fallthrough */
		case eGridStatus::SEARCHED:
			/* intentional fallthrough */
		case eGridStatus::VISITED:
			{
				if (_grid[pos._y * _width + pos._x] != eGridStatus::START && _grid[pos._y * _width + pos._x] != eGridStatus::DEST)
				{
					_grid[pos._y * _width + pos._x] = status;
				}
				break;
			}
		case eGridStatus::START:
			{
				if (_grid[pos._y * _width + pos._x] != eGridStatus::DEST)
				{
					_grid[_startPos._y * _width + _startPos._x] = eGridStatus::NORMAL;
					_grid[pos._y * _width + pos._x] = status;
					_startPos = { pos._x, pos._y };
				}
				break;
			}
		case eGridStatus::DEST:
			{
				if (_grid[pos._y * _width + pos._x] != eGridStatus::START)
				{
					_grid[_destPos._y * _width + _destPos._x] = eGridStatus::NORMAL;
					_grid[pos._y * _width + pos._x] = status;
					_destPos = { pos._x, pos._y };
				}
				break;
			}
		default:
			break;
		}
	}

	eGridStatus Map::GetValue(const int x, const int y) const
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return eGridStatus::INVALID;
		}

		return _grid[_width * y + x];
	}

	eGridStatus Map::GetValue(const Vector2 pos) const
	{
		if (pos._y >= _depth || pos._x >= _width || pos._y < 0 || pos._x < 0)
		{
			return eGridStatus::INVALID;
		}

		return _grid[_width * pos._y + pos._x];
	}

	void Map::ResetMap(void)
	{
		for (int iCnt = 0; iCnt < _width * _depth; iCnt++)
		{
			if (_grid[iCnt] == eGridStatus::SEARCHED || _grid[iCnt] == eGridStatus::VISITED)
			{
				_grid[iCnt] = eGridStatus::NORMAL;
			}
		}
	}

	void Map::RemoveAllObstacles(void)
	{
		for (int iCnt = 0; iCnt < _width * _depth; iCnt++)
		{
			if (_grid[iCnt] == eGridStatus::BLOCKED)
			{
				_grid[iCnt] = eGridStatus::NORMAL;
			}
		}
	}
}