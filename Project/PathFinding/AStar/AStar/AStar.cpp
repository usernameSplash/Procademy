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
	}

	void AStar::PathFind()
	{
		if (_openList.empty())
		{
			_bPathFound = false;
			goto FAILED_PATH_FINDING;
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
				goto FAILED_PATH_FINDING;
			}
		}

		if (_map->_grid[index] == eGridStatus::DEST)
		{
			_bPathFound = true;
			_closeList.push_back(curNode);
			_destNode = curNode;
			goto FINISHED_PATH_FINDING;
		}

		if (_map->_grid[index] == eGridStatus::SEARCHED)
		{
			_map->_grid[index] = eGridStatus::VISITED;
			_closeList.push_back(curNode);
		}

		for (int iCnt = 0; iCnt < 8; iCnt++)
		{
			int dx = curNode->_pos._x + _deltaPos[iCnt]._x;
			int dy = curNode->_pos._y + _deltaPos[iCnt]._y;

			if (dy < 0 || dy >= _map->_depth || dx < 0 || dx >= _map->_width)
			{
				continue;
			}

			int deltaIndex = dy * _map->_width + dx;

			if (_map->_grid[deltaIndex] == eGridStatus::NORMAL || _map->_grid[deltaIndex] == eGridStatus::SEARCHED)
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
			else if (_map->_grid[deltaIndex] == eGridStatus::VISITED)
			{
				Vector2 newPos { dx, dy };
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
			else if (_map->_grid[deltaIndex] == eGridStatus::DEST)
			{
				Vector2 newPos { dx, dy };
				Node* newNode = new Node(newPos, 0, 0, static_cast<Dir>(iCnt), curNode);
				_openList.push(newNode);
				break;
			}
		}

		return;

	FAILED_PATH_FINDING:
		_bPathFound = false;
	FINISHED_PATH_FINDING:
		_bPathFinding = false;
		_bTriedLastTime = true;
		return;
	}
}