#pragma once

namespace MyDataStructure
{
	class BinarySearchTree
	{
	protected:
		struct Node
		{
			Node* _pLeft;
			Node* _pRight;
			int _val;
		};

	public:
		BinarySearchTree(void);
		virtual ~BinarySearchTree(void);

		bool Insert(const int val);
		bool Delete(const int val);
		bool Contain(const int val) const;
		void GetArray(int* const outArray, const size_t len) const;

		size_t Size(void) const;

		void PrintTree(void) const;

	protected:
		void SearchNode(Node** const outNode, Node** const outParentNode, const int val) const;
		void GetArrayInternal(int* const outArray, const size_t len, const Node* const curNode, size_t* const index) const;
		virtual void PrintTreeInternal(const Node* const curNode) const;
		void Terminate(const Node* const curNode) const;

	protected:
		Node* _root;
		size_t _size;
	};
}