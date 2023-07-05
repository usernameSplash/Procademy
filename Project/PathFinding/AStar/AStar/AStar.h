#pragma once

#include "PathFinder.h"

namespace PathFinder
{
	class AStar : public PathFinder
	{
	public: 
		AStar(Map* map);
		virtual	~AStar();

		virtual void PathFind(void) override;
	};
}