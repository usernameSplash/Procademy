#include "BinarySearchTree.h"

#include <cstdio>
#include <cassert>

namespace MyDataStructure
{
	BinarySearchTree::BinarySearchTree(void)
		: _root(nullptr)
		, _size(0)
	{

	}

	BinarySearchTree::~BinarySearchTree(void)
	{
		Terminate(_root);
	}

	bool BinarySearchTree::Insert(const int val)
	{
		if (_root == nullptr)
		{
			_root = new Node;
			_root->_val = val;
			_root->_pLeft = nullptr;
			_root->_pRight = nullptr;

			_size++;
			return true;
		}

		Node* curNode;
		Node* curParentNode;
		SearchNode(&curNode, &curParentNode, val);

		if (curNode != nullptr)
		{
			return false; // Not Allow Duplicated Value in Tree
		}

		Node* newNode = new Node;
		newNode->_pLeft = nullptr;
		newNode->_pRight = nullptr;
		newNode->_val = val;

		if (curParentNode->_val > val)
		{
			curParentNode->_pLeft = newNode;
		}
		else
		{
			curParentNode->_pRight = newNode;
		}

		_size++;
		return true;

		/*
		while (true)
		{
			if (curNode->_val > val)
			{
				if (curNode->_pLeft == nullptr)
				{
					curNode->_pLeft = new Node;
					curNode->_pLeft->_val = val;
					_size++;
					return true;
				}
				curNode = curNode->_pLeft;
			}
			else if(curNode->_val < val)
			{
				if (curNode->_pRight == nullptr)
				{
					curNode->_pRight = new Node;
					curNode->_pRight->_val = val;
					_size++;
					return true;
				}
				curNode = curNode->_pRight;
			}
			else
			{
				return false; // Not Allow Duplicated Value in Tree
			}
		}
		*/
	}

	bool BinarySearchTree::Delete(const int val)
	{
		Node* targetNode;
		Node* targetParentNode;

		SearchNode(&targetNode, &targetParentNode, val);

		if (targetNode == nullptr)
		{
			return false;
		}

		if (targetNode->_pLeft == nullptr && targetNode->_pRight == nullptr)
		{
			if (targetNode == _root)
			{
				_root = nullptr;
			}

			if (targetNode->_val > targetParentNode->_val)
			{
				targetParentNode->_pRight = nullptr;
			}
			else
			{
				targetParentNode->_pLeft = nullptr;
			}

			delete targetNode;
		}
		else if (targetNode->_pLeft == nullptr)
		{
			if (targetNode == _root)
			{
				_root = targetNode->_pRight;
			}

			if (targetNode->_val > targetParentNode->_val)
			{
				targetParentNode->_pRight = targetNode->_pRight;
			}
			else
			{
				targetParentNode->_pLeft = targetNode->_pRight;
			}
			delete targetNode;
		}
		else if (targetNode->_pRight == nullptr)
		{
			if (targetNode == _root)
			{
				_root = targetNode->_pLeft;
			}

			if (targetNode->_val > targetParentNode->_val)
			{
				targetParentNode->_pRight = targetNode->_pLeft;
			}
			else
			{
				targetParentNode->_pLeft = targetNode->_pLeft;
			}
			delete targetNode;
		}
		else
		{
			Node* nextParentNode = targetNode;
			Node* nextNode = targetNode->_pRight;

			while (nextNode->_pLeft != nullptr)
			{
				nextParentNode = nextNode;
				nextNode = nextNode->_pLeft; 
			}

			targetNode->_val = nextNode->_val;

			if (nextParentNode == targetNode)
			{
				nextParentNode->_pRight == nullptr;
			}
			else
			{
				nextParentNode->_pLeft = nullptr;
			}

			delete nextNode;
		}

		_size--;

		return true;
	}

	bool BinarySearchTree::Contain(const int val) const
	{
		Node* node;
		Node* parentNode;
		SearchNode(&node, &parentNode, val);

		return (node != nullptr);
	}

	void BinarySearchTree::GetArray(int* const outArray, const size_t len) const
	{
		size_t index = 0;
		GetArrayInternal(outArray, len, _root, &index);
		return;
	}

	void BinarySearchTree::PrintTree(void) const
	{
		PrintTreeInternal(_root);
		wprintf(L"\n");
		return;
	}

	void BinarySearchTree::SearchNode(Node** const outNode, Node** const outParentNode, const int val) const
	{
		Node* curNode = _root;

		while (curNode != nullptr)
		{
			if (curNode->_val > val)
			{
				*outParentNode = curNode;
				curNode = curNode->_pLeft;
			}
			else if (curNode->_val < val)
			{
				*outParentNode = curNode;
				curNode = curNode->_pRight;
			}
			else
			{
				break;
			}
		}
		
		*outNode = curNode;

		return;
	}

	void BinarySearchTree::GetArrayInternal(int* const outArray, const size_t len, const Node* const curNode, size_t* const index) const
	{
		if (curNode == nullptr)
		{
			return;
		}

		if (*index >= len)
		{
			return;
		}

		GetArrayInternal(outArray, len, curNode->_pLeft, index);

		outArray[*index] = curNode->_val;
		(*index)++;
		
		GetArrayInternal(outArray, len, curNode->_pRight, index);

		return;
	}

	void BinarySearchTree::PrintTreeInternal(const Node* const curNode) const
	{
		if (curNode == nullptr)
		{
			return;
		}

		PrintTreeInternal(curNode->_pLeft);

		wprintf(L"%d ", curNode->_val);

		PrintTreeInternal(curNode->_pRight);

		return;
	}

	void BinarySearchTree::Terminate(const Node* const curNode) const
	{
		if (curNode == nullptr)
		{
			return;
		}

		Terminate(curNode->_pLeft);
		Terminate(curNode->_pRight);

		delete curNode;

		return;
	}
}