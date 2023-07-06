#pragma once
#include "Map.h"
#include "PathFinder.h"

#include <vector>
#include <Windows.h>

namespace PathFinder
{
	class MapController
	{
	public:
		MapController(Map* map, PathFinder* pathFinder, unsigned int gridSize = 16);
		~MapController();

		void BindPathFinder(PathFinder* pathFinder);

		void StartDrag(const int x, const int y);
		void EndDrag(void);
		bool IsDragging(void);

		void SetStartGrid(const int x, const int y);
		void SetDestGrid(const int x, const int y);
		void SetGrid(const int x, const int y);

		void Render(const HDC hdc) const;

	private:
		void RenderGrid(const HDC hdc) const;
		void RenderRectangle(const HDC hdc) const;
		void RenderPath(const HDC hdc) const;
		void RenderCorner(const HDC hdc) const;
		void RenderParentLine(const HDC hdc) const;

	private:
		Map* _map;
		PathFinder* _pathFinder;

		unsigned int _gridSize;
		bool _bObstacleDraw;
		bool _bObstacleErase;
		
	private:
		HPEN _gridPen;
		HPEN _pathPen;
		HPEN _parentLinePen;

		HBRUSH _normalBrush;
		HBRUSH _blockedBrush;
		HBRUSH _searchedBrush;
		HBRUSH _visitedBrush;
		HBRUSH _cornerBrush;
		HBRUSH _startBrush;
		HBRUSH _destBrush;
	};
}