#pragma once

namespace MyDataStructure
{
	class RedBlackTree
	{
	protected:
		enum class eNodeColor
		{
			Red = 0,
			Black
		};
		struct Node
		{
			Node* _pLeft {};
			Node* _pRight {};
			Node* _pParent {};
			int _val {};
			eNodeColor _color {};
		};

	public:
		RedBlackTree(void);
		virtual ~RedBlackTree(void);

		bool Insert(const int val);
		bool Delete(const int val);
		bool Contain(const int val) const;
		void GetArray(int* const outArray, const size_t len) const;

		size_t Size(void) const;

		void PrintTree(void) const;

	protected:
		void SearchNode(Node** const outNode, Node** const outParentNode, const int val) const;

		void RebalanceInsert(Node* node);
		void RebalanceDelete(Node* node);

		void LeftSpin(Node* const node);
		void RightSpin(Node* const node);

		void GetArrayInternal(int* const outArray, const size_t len, const Node* const curNode, size_t* const index) const;
		virtual void PrintTreeInternal(const Node* const curNode, int& order, const int depth) const;
		void Terminate(const Node* const curNode) const;

	protected:
		Node* _root;
		Node* _nil;
		size_t _size;
	};
}