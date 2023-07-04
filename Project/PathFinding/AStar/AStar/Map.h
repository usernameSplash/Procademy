#pragma once

#include "NodeHeap.h"

#include <vector>

namespace PathFinder
{
	constexpr size_t GRID_MINIMUM_SIZE = 8;
	constexpr size_t GRID_MAXIMUM_SIZE = 128;

	enum class GridStatus
	{
		NORMAL = 0,
		BLOCKED,
		SEARCHED,
		VISITED,
		START,
		DEST,
		INVALID
	};

	class Map
	{
	public:
		Map(const size_t width, const size_t depth);
		~Map();

		void SetValue(const size_t x, const size_t y, const GridStatus status);
		GridStatus GetValue(const size_t x, const size_t y) const;

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
		std::vector<GridStatus> _grid;
		size_t _width;
		size_t _depth;
		size_t _gridSize;
		Node* _startNode;
		Node* _destNode;

		friend class AStar;
	};
}