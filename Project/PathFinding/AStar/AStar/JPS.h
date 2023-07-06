#pragma once

#include "PathFinder.h"

namespace PathFinder
{
	class JPS : public PathFinder
	{
	public:
		JPS(Map* map);
		virtual ~JPS();

		virtual void PathFind(void) override;

	private:
		void CreateNode(Node* node);
		//void CreateNodeOnCorner(Node* node, const Dir dir);
		bool FindCorner(Node* node, const Dir searchDir, Vector2& outCornerPos);

		void AddCorner(Vector2 pos);
		void ResetCornerList(void);
	};
}