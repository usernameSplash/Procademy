#pragma once

#include "NodeHeap.h"
#include "Map.h"

namespace PathFinder
{
	class AStar
	{

	public: 
		AStar(Map* map);
		~AStar();

		void PathFind(void);
		void Render(void);

		void DeallocAllNodes(void);

	private:
		Map* _map;
		NodeHeap _openList;
		std::vector<Node*> _closeList;
		std::vector<GridStatus> _grid;
	};
}