#include "Map.h"

namespace PathFinder
{
	Map::Map(const size_t width, const size_t depth)
		: _bUpdated(false)
		, _width(width)
		, _depth(depth)
		, _gridSize(16)
		, _startNode(nullptr)
		, _destNode(nullptr)
	{
		_grid.reserve(_width * _depth);
		_grid.resize(_width * _depth, 0);
	}

	Map::~Map()
	{
		if (_startNode)
		{
			delete _startNode;
		}

		if (_destNode)
		{
			delete _destNode;
		}
	}

	void Map::SetValue(const size_t x, const size_t y, const int value)
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return;
		}

		_bUpdated = true;
		_grid[_width * y + x] = value;
	}

	int Map::GetValue(const size_t x, const size_t y) const
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return -2;
		}

		return _grid[_width * y + x];
	}

	void Map::GridZoomIn(void)
	{
		if (_gridSize < GRID_MAXIMUM_SIZE)
		{
			_gridSize++;
		}
	}

	void Map::GridZoomOut(void)
	{
		if (_gridSize > GRID_MINIMUM_SIZE)
		{
			_gridSize--;
		}
	}

	size_t Map::GridSize(void) const
	{
		return _gridSize;
	}
	
	void Map::SetStartNode(const int x, const int y)
	{
		if (_startNode == nullptr)
		{
			_startNode = new Node();
		}

		_startNode->SetPos(x, y);
		SetValue(_startNode->_x, _startNode->_y, GRID_START);
	}

	void Map::SetDestNode(const int x, const int y)
	{
		if (_destNode == nullptr)
		{
			_destNode = new Node();
		}

		_destNode->SetPos(x, y);
		SetValue(_destNode->_x, _destNode->_y, GRID_DEST);
	}

	void Map::ResetStartDestNode(void)
	{
		SetValue(_startNode->_x, _startNode->_y, GRID_NORMAL);
		SetValue(_destNode->_x, _destNode->_y, GRID_NORMAL);

		delete _startNode;
		delete _destNode;

		_startNode = nullptr;
		_destNode = nullptr;
	}

	bool Map::IsSetStartNode(void) const
	{
		return (_startNode != nullptr);
	}

	bool Map::IsSetDestNode(void) const
	{
		return (_destNode != nullptr);
	}
}