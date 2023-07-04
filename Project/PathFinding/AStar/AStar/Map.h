#pragma once

#include "NodeHeap.h"

#include <vector>

namespace PathFinder
{
	constexpr size_t GRID_MINIMUM_SIZE = 8;
	constexpr size_t GRID_MAXIMUM_SIZE = 128;

#define GRID_NORMAL 0
#define GRID_START -1
#define GRID_DEST -2
#define GRID_BLOCKED -3
#define GRID_SEARCHED -4
#define GRID_INVALID -5

	class Map
	{
	public:
		Map(const size_t width, const size_t depth);
		~Map();

		void SetValue(const size_t x, const size_t y, const int status);
		int GetValue(const size_t x, const size_t y) const;

		void GridZoomIn(void);
		void GridZoomOut(void);
		size_t GridSize(void) const;

		void SetStartNode(const int x, const int y);
		void SetDestNode(const int x, const int y);
		void ResetStartDestNode(void);

		bool IsSetStartNode(void) const;
		bool IsSetDestNode(void) const;

	private:
		bool _bUpdated;
		std::vector<int> _grid;
		size_t _width;
		size_t _depth;
		size_t _gridSize;
		Node* _startNode;
		Node* _destNode;

		friend class AStar;
	};
}