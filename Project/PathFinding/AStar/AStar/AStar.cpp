#include "AStar.h"

namespace PathFinder
{
	AStar::AStar(Map* map)
		: _map(map)
		, _openList()
		, _closeList()
	{
		_closeList.reserve(4096);
	}

	AStar::~AStar()
	{
		DeallocAllNodes();
	}

	void AStar::PathFind(void)
	{
		char deltaPos[8][2] = {
			{-1, 0},
			{0, -1},
			{1, 0},
			{0, 1},
			{-1, -1},
			{1, -1},
			{1, 1},
			{-1, 1}
		};

		if (_openList.empty() == false)
		{
			DeallocAllNodes(); // if previous result is left, delete that.
		}

		_grid = _map->_grid;
		_openList.push(_map->_startNode);

		while (_openList.empty() == false)
		{
			Node* curNode = _openList.top();
			_openList.pop();
			_closeList.push_back(curNode);

			for (size_t iCnt = 0; iCnt < 8; iCnt++)
			{
				size_t index = curNode->_y * _map->_width + curNode->_x;
				if (_grid[index] != GridStatus::VISITED || _grid[index] != GridStatus::BLOCKED)
				{
					_grid[index] = GridStatus::VISITED;

					Node* newNode = new Node;
					newNode->Set(curNode, _map->_destNode, static_cast<Dir>(iCnt));

					_openList.push(newNode);
				}
			}
		}
	}

	void AStar::DeallocAllNodes(void)
	{
		_openList.Clear();

		for (auto it = _closeList.begin(); it != _closeList.end(); ++it)
		{
			delete (*it);
		}

		_closeList.clear();
	}
}