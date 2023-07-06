#pragma once

#include "NodeHeap.h"
#include "Map.h"

#include <vector>

namespace PathFinder
{
	class PathFinder
	{
	public:
		PathFinder(Map* map);
		virtual ~PathFinder();

		void DeallocAllNodes(void);
		bool IsPathFound(void) const; 
		bool IsPathFinding(void) const;
		bool IsTriedLastTime(void) const;

		void StartPathFinding(void);
		void EndPathFinding(void);

		void SetMap(Map* map);

		Node* GetStartNode(void) const;
		Node* GetDestNode(void) const;

		const NodeHeap& GetOpenList(void) const;
		const std::vector<Node*> GetCloseList(void) const;
		const std::vector<Vector2>& GetCornerList(void) const;

		virtual void PathFind(void) = 0;

	protected:
		Vector2 _deltaPos[8] = {
			Vector2{-1, 0},
			Vector2{0, -1},
			Vector2{1, 0},
			Vector2{0, 1},
			Vector2{-1, -1},
			Vector2{1, -1},
			Vector2{1, 1},
			Vector2{-1, 1}
		};

	protected:
		bool _bPathFound;
		bool _bPathFinding;
		bool _bTriedLastTime;

		Map* _map;
		Node* _startNode;
		Node* _destNode;

		NodeHeap _openList;
		std::vector<Node*> _closeList;
		std::vector<Vector2> _cornerList;
	};
}