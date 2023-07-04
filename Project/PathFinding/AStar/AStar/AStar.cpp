#include "AStar.h"
#include <synchapi.h>
#include <cstdio>

using namespace std;

namespace PathFinder
{
	AStar::AStar(Map* map, HWND hWnd)
		: _bPathFound(false)
		, _map(map)
		, _openList()
		, _closeList()
		, _hWnd(hWnd)
	{
		_closeList.reserve(4096);

		_grid.resize(_map->_grid.size(), GRID_NORMAL);

		_gridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		_pathPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

		_normalBrush = CreateSolidBrush(RGB(255, 255, 255));
		_blockedBrush = CreateSolidBrush(RGB(100, 100, 100));
		_searchedBrush = CreateSolidBrush(RGB(0, 0, 255));
		_visitedBrush = CreateSolidBrush(RGB(255, 255, 0));
		_startBrush = CreateSolidBrush(RGB(0, 255, 0));
		_destBrush = CreateSolidBrush(RGB(255, 0, 0));
	}

	AStar::~AStar()
	{
		DeleteObject(_gridPen);
		DeleteObject(_normalBrush);
		DeleteObject(_visitedBrush);
		DeleteObject(_blockedBrush);
		DeleteObject(_startBrush);
		DeleteObject(_destBrush);

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

		Node* destNode = _map->_destNode;

		if (!_map->IsSetStartNode() || !_map->IsSetDestNode())
		{
			return;
		}

		if (_openList.empty() == false || _closeList.empty() == false)
		{
			DeallocAllNodes(); // if previous result is left, delete that.
		}

		_bPathFound = false;
		_bPathFinding = true;

		_map->_startNode->Set(nullptr, destNode, Dir::NONE);
		_openList.push(_map->_startNode);
		
		while (_openList.empty() == false)
		{
			Node* curNode = _openList.top();
			_openList.pop();

			FILE* file;
			fopen_s(&file, "pathfindresult.txt", "a");
			fprintf(file, "X : %d, Y : %d, G : %d, H : %d, F : %d\n", curNode->_x, curNode->_y, curNode->_g, curNode->_h, curNode->_f);
			fclose(file);

			size_t index = curNode->_y * _map->_width + curNode->_x;

			if (_grid[index] > 0)
			{
				continue; // pass already visited node
			}
			else if (_grid[index] == GRID_SEARCHED)
			{
				_grid[index] = curNode->_f;
				_closeList.push_back(curNode);
			}

			for (size_t iCnt = 0; iCnt < 8; iCnt++)
			{
				size_t dx = curNode->_x + deltaPos[iCnt][0];
				size_t dy = curNode->_y + deltaPos[iCnt][1];

				if (dy < 0 || dy >= _map->_depth || dx < 0 || dx >= _map->_width)
				{
					continue;
				}

				size_t deltaIndex = dy * _map->_width + dx;

				if (_grid[deltaIndex] == GRID_DEST)
				{
					destNode->Set(curNode, destNode, static_cast<Dir>(iCnt));
					_bPathFound = true;
					goto break_loop;
				}
				else if (_grid[deltaIndex] == GRID_NORMAL || _grid[deltaIndex] == GRID_SEARCHED)
				{
					_grid[deltaIndex] = GRID_SEARCHED;

					Node* newNode = new Node;
					newNode->Set(curNode, destNode, static_cast<Dir>(iCnt));

					_openList.push(newNode);
				}
				else if (_grid[deltaIndex] > 0)
				{
					int newF = curNode->_f;
					if (iCnt >= static_cast<int>(Dir::LU)) // if Diagnal
					{
						newF += Node::DIAGNAL;
					}
					else
					{
						newF += Node::COMMON;
					}

					if (_grid[deltaIndex] > newF)
					{
						Node* newNode = new Node;
						newNode->Set(curNode, destNode, static_cast<Dir>(iCnt));

						_openList.push(newNode);
					}
				}
			}

			InvalidateRect(_hWnd, NULL, true);
			UpdateWindow(_hWnd);
			Sleep(2);

			continue;

		break_loop:
			InvalidateRect(_hWnd, NULL, true);
			UpdateWindow(_hWnd);
			break;
		}

		_bPathFinding = false;
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

	void AStar::Render(const HDC hdc)
	{
		UpdateMap();

		RenderRectangle(hdc);
		RenderGrid(hdc);
		
		if (_bPathFound)
		{
			RenderPath(hdc);
		}
	}

	void AStar::RenderGrid(const HDC hdc) const
	{
		int x = 0;
		int y = 0;
		HPEN hOldPen = (HPEN)SelectObject(hdc, _gridPen);

		const size_t WIDTH = _map->_width;
		const size_t DEPTH = _map->_depth;
		const size_t GRID_SIZE = _map->_gridSize;

		for (int widthCount = 0; widthCount <= WIDTH; widthCount++)
		{
			MoveToEx(hdc, x, 0, NULL);
			LineTo(hdc, x, DEPTH * GRID_SIZE);
			x += GRID_SIZE;
		}

		for (int heightCount = 0; heightCount <= DEPTH; heightCount++)
		{
			MoveToEx(hdc, 0, y, NULL);
			LineTo(hdc, WIDTH * GRID_SIZE, y);
			y += GRID_SIZE;
		}

		SelectObject(hdc, hOldPen);
	}

	void AStar::RenderRectangle(const HDC hdc) const
	{
		int x;
		int y;

		SelectObject(hdc, GetStockObject(NULL_PEN));

		const size_t WIDTH = _map->_width;
		const size_t DEPTH = _map->_depth;
		const size_t GRID_SIZE = _map->_gridSize;

		HBRUSH oldBrush;

		for (size_t depthCount = 0; depthCount < DEPTH; depthCount++)
		{
			for (size_t widthCount = 0; widthCount < WIDTH; widthCount++)
			{
				switch (_grid[depthCount * WIDTH + widthCount])
				{
				case GRID_NORMAL:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _normalBrush);
						break;
					}
				case GRID_START:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _startBrush);
						break;
					}
				case GRID_DEST:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _destBrush);
						break;
					}
				case GRID_BLOCKED:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _blockedBrush);
						break;
					}
				case GRID_SEARCHED:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _searchedBrush);
						break;
					}
				default:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _visitedBrush);
						break;
					}
				}
			
				x = static_cast<int>(widthCount * GRID_SIZE);
				y = static_cast<int>(depthCount * GRID_SIZE);
				Rectangle(hdc, x, y, x + GRID_SIZE + 2, y + GRID_SIZE + 2);
				SelectObject(hdc, oldBrush);
			}
		}
	}

	void AStar::RenderPath(const HDC hdc) const
	{
		const Node* curNode = _map->_destNode;

		HPEN oldPen = (HPEN)SelectObject(hdc, _pathPen);

		while (true)
		{
			Node* parentNode = curNode->_from;

			if (parentNode == nullptr)
			{
				break;
			}

			MoveToEx(hdc, curNode->_x * _map->_gridSize + (_map->_gridSize) / 2, curNode->_y * _map->_gridSize + (_map->_gridSize) / 2, NULL);
			LineTo(hdc, parentNode->_x * _map->_gridSize + (_map->_gridSize) / 2, parentNode->_y * _map->_gridSize + (_map->_gridSize) / 2);

			curNode = curNode->_from;
		}

		SelectObject(hdc, oldPen);
	}

	void AStar::UpdateMap(void)
	{
		if (_map->_bUpdated)
		{
			_grid = _map->_grid;

			_bPathFound = false;
			_bPathFinding = false;
		}
		_map->_bUpdated = false;
	}
}