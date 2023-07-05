#pragma once
#include <queue>
#include "Vector2.h"

namespace PathFinder
{
	enum class Dir
	{
		LL = 0,
		UU,
		RR,
		DD,
		LU,
		RU,
		RD,
		LD,
		NONE
	};

	class Node
	{
	public:
		Node(const int x, const int y, const int g, const int h, const Dir dir, Node* parentNode);
		Node(const Vector2 pos, const int g, const int h, const Dir dir, Node* parentNode);

	public:
		Vector2 _pos;
		unsigned int _g {};
		unsigned int _h {};
		unsigned int _f {};
		Node* _parent {};
		Dir _dir {};

		friend class NodeHeap;
	};

	struct NodePointerComparer
	{
		bool operator()(const Node* a, const Node* b) const
		{
			return a->_f > b->_f;
		}
	};

	class NodeHeap : public std::priority_queue<Node*, std::vector<Node*>, NodePointerComparer>
	{
	public:
		NodeHeap(const size_t size = 4096);
		~NodeHeap() = default;
		
		void Clear(void);
	};
}