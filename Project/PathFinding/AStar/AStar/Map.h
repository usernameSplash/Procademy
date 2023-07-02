#pragma once

#include "NodeHeap.h"

#include <vector>

namespace PathFinder
{
	enum class GridStatus
	{
		NORMAL = 0,
		VISITED,
		BLOCKED,
		START,
		DEST
	};

	class Map
	{
	public:
		Map(const size_t width, const size_t depth);
		~Map() = default;

		void SetValue(const size_t x, const size_t y, bool value);
		bool GetValue(const size_t x, const size_t y) const;

	private:
		std::vector<GridStatus> _grid;
		size_t _width;
		size_t _depth;
		Node* _startNode;
		Node* _destNode;

		friend class AStar;
	};
}