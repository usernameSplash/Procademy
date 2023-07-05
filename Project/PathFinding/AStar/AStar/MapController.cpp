#include "MapController.h"

namespace PathFinder
{
	MapController::MapController(Map* map, PathFinder* pathFinder, unsigned int gridSize)
		: _map(map)
		, _pathFinder(pathFinder)
		, _gridSize(gridSize)
		, _bObstacleDraw(false)
		, _bObstacleErase(false)
	{
		_gridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		_pathPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

		_normalBrush = CreateSolidBrush(RGB(255, 255, 255));
		_blockedBrush = CreateSolidBrush(RGB(100, 100, 100));
		_searchedBrush = CreateSolidBrush(RGB(0, 0, 255));
		_visitedBrush = CreateSolidBrush(RGB(255, 255, 0));
		_startBrush = CreateSolidBrush(RGB(0, 255, 0));
		_destBrush = CreateSolidBrush(RGB(255, 0, 0));
	}

	MapController::~MapController()
	{
		DeleteObject(_gridPen);
		DeleteObject(_normalBrush);
		DeleteObject(_visitedBrush);
		DeleteObject(_blockedBrush);
		DeleteObject(_startBrush);
		DeleteObject(_destBrush);
	}

	void MapController::StartDrag(const int x, const int y)
	{
		const int gridX = x / _gridSize;
		const int gridY = y / _gridSize;

		if (_map->GetValue(gridX, gridY) == eGridStatus::BLOCKED)
		{
			_bObstacleDraw = false;
			_bObstacleErase = true;
		}
		else
		{
			_bObstacleDraw = true;
			_bObstacleErase = false;
		}

	}

	void MapController::EndDrag(void)
	{
		_bObstacleDraw = false;
		_bObstacleErase = false;
	}

	bool MapController::IsDragging(void)
	{
		return _bObstacleDraw || _bObstacleErase;
	}

	void MapController::SetStartGrid(const int x, const int y)
	{
		const int gridX = x / _gridSize;
		const int gridY = y / _gridSize;

		_map->SetValue(gridX, gridY, eGridStatus::START);
	}

	void MapController::SetDestGrid(const int x, const int y)
	{
		const int gridX = x / _gridSize;
		const int gridY = y / _gridSize;

		_map->SetValue(gridX, gridY, eGridStatus::DEST);
	}

	void MapController::SetGrid(const int x, const int y)
	{
		const int gridX = x / _gridSize;
		const int gridY = y / _gridSize;

		if (_bObstacleDraw)
		{
			_map->SetValue(gridX, gridY, eGridStatus::BLOCKED);
		}
		else if (_bObstacleErase && _map->GetValue(gridX, gridY) == eGridStatus::BLOCKED)
		{
			_map->SetValue(gridX, gridY, eGridStatus::NORMAL);
		}
	}

	void MapController::Render(const HDC hdc) const
	{
		RenderRectangle(hdc);
		RenderGrid(hdc);

		if (_pathFinder->IsPathFound())
		{
			RenderPath(hdc);
		}
	}

	void MapController::RenderGrid(const HDC hdc) const
	{
		int x = 0;
		int y = 0;
		HPEN hOldPen = (HPEN)SelectObject(hdc, _gridPen);

		const int WIDTH = _map->_width;
		const int DEPTH = _map->_depth;

		for (int widthCount = 0; widthCount <= WIDTH; widthCount++)
		{
			MoveToEx(hdc, x, 0, NULL);
			LineTo(hdc, x, DEPTH * _gridSize);
			x += _gridSize;
		}

		for (int heightCount = 0; heightCount <= DEPTH; heightCount++)
		{
			MoveToEx(hdc, 0, y, NULL);
			LineTo(hdc, WIDTH * _gridSize, y);
			y += _gridSize;
		}

		SelectObject(hdc, hOldPen);
	}

	void MapController::RenderRectangle(const HDC hdc) const
	{
		int x;
		int y;

		SelectObject(hdc, GetStockObject(NULL_PEN));

		const size_t WIDTH = _map->_width;
		const size_t DEPTH = _map->_depth;

		HBRUSH oldBrush;

		for (size_t depthCount = 0; depthCount < DEPTH; depthCount++)
		{
			for (size_t widthCount = 0; widthCount < WIDTH; widthCount++)
			{
				switch (_map->_grid[depthCount * WIDTH + widthCount])
				{
				case eGridStatus::NORMAL:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _normalBrush);
						break;
					}
				case eGridStatus::BLOCKED:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _blockedBrush);
						break;
					}
				case eGridStatus::SEARCHED:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _searchedBrush);
						break;
					}
				case eGridStatus::VISITED:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _visitedBrush);
						break;
					}
				case eGridStatus::START:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _startBrush);
						break;
					}
				case eGridStatus::DEST:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _destBrush);
						break;
					}
				default:
					{
						oldBrush = (HBRUSH)SelectObject(hdc, _normalBrush);
						break;
					}
				}

				x = static_cast<int>(widthCount * _gridSize);
				y = static_cast<int>(depthCount * _gridSize);
				Rectangle(hdc, x, y, x + _gridSize + 2, y + _gridSize + 2);
				SelectObject(hdc, oldBrush);
			}
		}
	}

	void MapController::RenderPath(const HDC hdc) const
	{
		const Node* curNode = _pathFinder->GetDestNode();

		HPEN oldPen = (HPEN)SelectObject(hdc, _pathPen);

		while (true)
		{
			Node* parentNode = curNode->_parent;

			if (parentNode == nullptr)
			{
				break;
			}

			MoveToEx(hdc, curNode->_pos._x * _gridSize + _gridSize / 2, curNode->_pos._y * _gridSize + _gridSize / 2, NULL);
			LineTo(hdc, parentNode->_pos._x * _gridSize + _gridSize / 2, parentNode->_pos._y * _gridSize + _gridSize / 2);

			curNode = curNode->_parent;
		}

		SelectObject(hdc, oldPen);
	}

}