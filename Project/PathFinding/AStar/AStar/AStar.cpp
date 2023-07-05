#include "AStar.h"

using namespace std;

namespace PathFinder
{
	AStar::AStar(Map* map)
		: PathFinder(map)
	{
	}

	AStar::~AStar()
	{
		DeallocAllNodes();
	}

	void AStar::PathFind()
	{
		Vector2 deltaPos[8] = {
			Vector2{-1, 0},
			Vector2{0, -1},
			Vector2{1, 0},
			Vector2{0, 1},
			Vector2{-1, -1},
			Vector2{1, -1},
			Vector2{1, 1},
			Vector2{-1, 1}
		};

		if (_openList.empty())
		{
			_bPathFound = false;
			goto FINISHED_PATH_FINDING;
		}

		Node* curNode;
		int index;

		while (true)
		{
			curNode = _openList.top();
			_openList.pop();

			index = curNode->_pos._y * _map->_width + curNode->_pos._x;
			if (_map->_grid[index] != eGridStatus::VISITED)
			{
				break;
			}
			else if (_openList.empty())
			{
				goto FINISHED_PATH_FINDING;
			}
		}

		if (_map->_grid[index] == eGridStatus::SEARCHED)
		{
			_map->_grid[index] = eGridStatus::VISITED;
			_closeList.push_back(curNode);
		}

		for (int iCnt = 0; iCnt < 8; iCnt++)
		{
			int dx = curNode->_pos._x + deltaPos[iCnt]._x;
			int dy = curNode->_pos._y + deltaPos[iCnt]._y;

			if (dy < 0 || dy >= _map->_depth || dx < 0 || dx >= _map->_width)
			{
				continue;
			}

			int deltaIndex = dy * _map->_width + dx;

			if (_map->_grid[deltaIndex] == eGridStatus::DEST)
			{
				_destNode = new Node(dx, dy, 0, 0, (Dir)iCnt, curNode);
				_bPathFound = true;
				goto FINISHED_PATH_FINDING;
			}
			else if (_map->_grid[deltaIndex] == eGridStatus::NORMAL || _map->_grid[deltaIndex] == eGridStatus::SEARCHED)
			{
				_map->_grid[deltaIndex] = eGridStatus::SEARCHED;

				Vector2 newPos { dx, dy };
				Node* newNode;

				if (iCnt >= static_cast<int>(Dir::LU))
				{
					newNode = new Node(newPos, curNode->_g + DISTANCE::DIAGNAL, newPos.DistanceManhattan(_map->_destPos), static_cast<Dir>(iCnt), curNode);
				}
				else
				{
					newNode = new Node(newPos, curNode->_g + DISTANCE::COMMON, newPos.DistanceManhattan(_map->_destPos), static_cast<Dir>(iCnt), curNode);
				}

				_openList.push(newNode);
			}
			else if (_map->_grid[deltaIndex] == eGridStatus::VISITED /* and weight check */ )
			{
				Vector2 newPos{dx, dy};
				int newG = curNode->_g;
					
				if (iCnt >= static_cast<int>(Dir::LU)) // if Diagnal
				{
					newG += DISTANCE::DIAGNAL;
				}
				else
				{
					newG += DISTANCE::COMMON;
				}

				int newH = newG + newPos.DistanceManhattan(_map->_destPos);

				if (_map->_gValueGrid[deltaIndex] > (newG + newH))
				{
					Node* newNode = new Node(newPos, newG, newH, static_cast<Dir>(iCnt), curNode);
					_openList.push(newNode);
				}
			}
		}

		return;

	FINISHED_PATH_FINDING:
		_bPathFinding = false;
		_bTriedLastTime = true;
		return;
	}
}