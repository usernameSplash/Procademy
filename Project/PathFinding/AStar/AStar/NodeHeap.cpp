#include "NodeHeap.h"

#include <cmath>

namespace PathFinder
{
	Node::Node(const int x, const int y, int g, int h, const Dir dir, Node* parentNode)
		: _pos(x, y)
		, _g(g)
		, _h(h)
		, _f(g + h)
		, _parent(parentNode)
		, _dir(dir)
	{
	}
	Node::Node(const Vector2 pos, int g, int h, const Dir dir, Node* parentNode)
		: _pos(pos)
		, _g(g)
		, _h(h)
		, _f(g + h)
		, _parent(parentNode)
		, _dir(dir)
	{
	}

	NodeHeap::NodeHeap(const size_t size)
	{
		c.reserve(size);
	}

	const std::vector<Node*>& NodeHeap::GetContainer(void) const
	{
		return c;
	}

	void NodeHeap::Clear(void)
	{
		for (auto it = c.begin(); it != c.end(); ++it)
		{
			delete (*it);
		}
		c.clear();
	}
}