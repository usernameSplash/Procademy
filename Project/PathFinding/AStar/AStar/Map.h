#pragma once

#include "Vector2.h"

#include <vector>

namespace PathFinder
{
	constexpr size_t GRID_MINIMUM_SIZE = 8;
	constexpr size_t GRID_MAXIMUM_SIZE = 128;

	enum class eGridStatus
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
		Map(const int width, const int depth);
		~Map();

		void SetValue(const int x, const int y, const eGridStatus status);
		eGridStatus GetValue(const int x, const int y) const;

		void ResetMap(void);
		void RemoveAllObstacles(void);

	public:
		Vector2 _startPos;
		Vector2 _destPos;

		int _width;
		int _depth;

	public:
		std::vector<eGridStatus> _grid;
		std::vector<eGridStatus> _oldGrid;
		std::vector<int> _gValueGrid;

		friend class MapController;
	};

}