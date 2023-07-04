#pragma once

#include "NodeHeap.h"
#include "Map.h"

#include <Windows.h>

namespace PathFinder
{
	class AStar
	{

	public: 
		AStar(Map* map, HWND hWnd);
		~AStar();

		void PathFind(void);
		void DeallocAllNodes(void);
		void Render(const HDC hdc);

	private:
		void RenderGrid(const HDC hdc) const;
		void RenderRectangle(const HDC hdc) const;
		void RenderPath(const HDC hdc) const;
		void UpdateMap(void);

	private:
		bool _bPathFound;
		bool _bPathFinding;

		Map* _map;
		NodeHeap _openList;
		std::vector<Node*> _closeList;
		std::vector<int> _grid;

		HWND _hWnd;
		HPEN _gridPen;
		HPEN _pathPen;
		HBRUSH _normalBrush;
		HBRUSH _blockedBrush;
		HBRUSH _searchedBrush;
		HBRUSH _visitedBrush;
		HBRUSH _startBrush;
		HBRUSH _destBrush;
	};
}