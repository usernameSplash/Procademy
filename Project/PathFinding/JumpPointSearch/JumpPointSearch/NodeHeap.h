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

	class NodeHeap
	{
	private:
		class Node
		{
		public:
			void Set(Node* parentNode, Node* endNode, Dir dir);

		private:
			enum Weight
			{
				COMMON = 100,
				DIAGNAL = 141
			};

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

	public:
		NodeHeap();
		~NodeHeap() = default;

	public:
		std::priority_queue<Node>* _heap;
	};
}