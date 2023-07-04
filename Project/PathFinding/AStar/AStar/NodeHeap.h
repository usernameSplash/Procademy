#pragma once
#include <queue>

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
		void SetPos(const int x, const int y);
		void Set(Node* parentNode, const Node* destNode, const Dir dir);

	public:
		enum Weight
		{
			COMMON = 100,
			DIAGNAL = 141
		};

	public:
		unsigned int _g {};
		unsigned int _h {};
		unsigned int _f {};
		int _x {};
		int _y {};
		Node* _from {};

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