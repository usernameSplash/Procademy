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
		LD
	};

	class Node
	{
	public:
		void Set(Node* parentNode, Node* destNode, Dir dir);

	private:
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

		bool operator<(const Node& other) const
		{
			return _h < other._h;
		}
	};

	class NodeHeap : public std::priority_queue<Node*>
	{
	private:
		struct NodeLess
		{
			bool operator()(const Node* node, const Node* anotherNode) const
			{
				return node->_f > anotherNode->_f;
			}
		};
	public:
		NodeHeap(const size_t size = 4096);
		~NodeHeap() = default;
		
		void Clear(void);
	};
}