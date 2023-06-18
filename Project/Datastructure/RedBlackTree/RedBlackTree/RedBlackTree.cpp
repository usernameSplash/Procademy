#include "RedBlackTree.h"
#include "ConsoleManager.h"

#include <cstdio>
#include <cassert>

namespace MyDataStructure
{
	RedBlackTree::RedBlackTree(void)
		: _size(0)
	{
		_nil = new Node();
		_nil->_pLeft = nullptr;
		_nil->_pRight = nullptr;
		_nil->_pParent = nullptr;
		_nil->_color = eNodeColor::Black;

		_root = _nil;
	}

	RedBlackTree::~RedBlackTree(void)
	{
		Terminate(_root);
		delete _nil;
	}

	bool RedBlackTree::Insert(const int val)
	{
		if (_root == _nil)
		{
			_root = new Node;
			_root->_pLeft = _nil;
			_root->_pRight = _nil;
			_root->_pParent = _nil;
			_root->_val = val;
			_root->_color = eNodeColor::Black;

			_size++;
			return true;
		}

		Node* curNode;
		Node* curParentNode;
		SearchNode(&curNode, &curParentNode, val);

		if (curNode != _nil)
		{
			return false; // Not Allow Duplicated Value in Tree
		}

		Node* newNode = new Node;
		newNode->_pLeft = _nil;
		newNode->_pRight = _nil;
		newNode->_pParent = curParentNode;
		newNode->_val = val;
		newNode->_color = eNodeColor::Red;

		if (curParentNode->_val > val)
		{
			curParentNode->_pLeft = newNode;
		}
		else
		{
			curParentNode->_pRight = newNode;
		}

		_size++;

		RebalanceInsert(newNode);

		return true;
	}

	bool RedBlackTree::Delete(const int val)
	{
		Node* targetNode;
		Node* targetParentNode;

		SearchNode(&targetNode, &targetParentNode, val);

		if (targetNode == _nil)
		{
			return false;
		}

		if (targetNode->_pLeft == _nil)
		{
			if (targetNode == _root)
			{
				_root = targetNode->_pRight;
			}
			else if (targetNode->_val > targetParentNode->_val)
			{
				targetParentNode->_pRight = targetNode->_pRight;
			}
			else
			{
				targetParentNode->_pLeft = targetNode->_pRight;
			}
			delete targetNode;
		}
		else if (targetNode->_pRight == _nil)
		{
			if (targetNode == _root)
			{
				_root = targetNode->_pLeft;
			}
			else if (targetNode->_val > targetParentNode->_val)
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

			while (nextNode->_pLeft != _nil)
			{
				nextParentNode = nextNode;
				nextNode = nextNode->_pLeft;
			}

			targetNode->_val = nextNode->_val;

			if (nextParentNode == targetNode)
			{
				nextParentNode->_pRight = _nil;
			}
			else
			{
				nextParentNode->_pLeft = _nil;
			}

			delete nextNode;
		}

		_size--;

		return true;
	}

	bool RedBlackTree::Contain(const int val) const
	{
		Node* node;
		Node* parentNode;
		SearchNode(&node, &parentNode, val);

		return (node != nullptr);
	}

	void RedBlackTree::GetArray(int* const outArray, const size_t len) const
	{
		size_t index = 0;
		GetArrayInternal(outArray, len, _root, &index);
		return;
	}

	size_t RedBlackTree::Size(void) const
	{
		return _size;
	}

	void RedBlackTree::PrintTree(void) const
	{
		int order = 0;

		PrintTreeInternal(_root, order, 0);
		return;
	}

	void RedBlackTree::SearchNode(Node** const outNode, Node** const outParentNode, const int val) const
	{
		Node* curNode = _root;

		while (curNode != _nil && curNode->_val != val)
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
		}

		*outNode = curNode;

		return;
	}

	void RedBlackTree::RebalanceInsert(Node* node)
	{
		while (node->_pParent->_color == eNodeColor::Red)
		{
			if (node->_pParent == node->_pParent->_pParent->_pLeft)
			{
				Node* uncle = node->_pParent->_pParent->_pRight;
				if (uncle->_color == eNodeColor::Red)
				{
					uncle->_color = eNodeColor::Black;
					node->_pParent->_color = eNodeColor::Black;
					node->_pParent->_pParent->_color = eNodeColor::Red;
					node = node->_pParent->_pParent;
				}
				else
				{
					if (node == node->_pParent->_pRight)
					{
						node = node->_pParent;
						LeftSpin(node);
					}

					node->_pParent->_color = eNodeColor::Black;
					node->_pParent->_pParent->_color = eNodeColor::Red;
					RightSpin(node->_pParent->_pParent);
				}
			}
			else
			{
				Node* uncle = node->_pParent->_pParent->_pLeft;
				if (uncle->_color == eNodeColor::Red)
				{
					uncle->_color = eNodeColor::Black;
					node->_pParent->_color = eNodeColor::Black;
					node->_pParent->_pParent->_color = eNodeColor::Red;
					node = node->_pParent->_pParent;
				}
				else
				{
					if (node == node->_pParent->_pLeft)
					{
						node = node->_pParent;
						RightSpin(node);
					}

					node->_pParent->_color = eNodeColor::Black;
					node->_pParent->_pParent->_color = eNodeColor::Red;
					LeftSpin(node->_pParent->_pParent);
				}
			}
		}

		_root->_color = eNodeColor::Black;
	}

	void RedBlackTree::RebalanceDelete(Node* node)
	{

	}

	void RedBlackTree::LeftSpin(Node* const node)
	{
		Node* rightChild = node->_pRight;
		
		node->_pRight = rightChild->_pLeft;

		if (rightChild->_pLeft != _nil)
		{
			rightChild->_pLeft->_pParent = node;
		}

		rightChild->_pParent = node->_pParent;
		if (_root == node)
		{
			_root = rightChild;
		}
		else if (node == node->_pParent->_pLeft)
		{
			node->_pParent->_pLeft = rightChild;
		}
		else
		{
			node->_pParent->_pRight = rightChild;
		}

		node->_pParent = rightChild;
		rightChild->_pLeft = node;
	}

	void RedBlackTree::RightSpin(Node* const node)
	{
		Node* leftChild = node->_pLeft;

		node->_pLeft = leftChild->_pRight;

		if (leftChild->_pRight != _nil)
		{
			leftChild->_pRight->_pParent = node;
		}

		leftChild->_pParent = node->_pParent;
		if (_root == node)
		{
			_root = leftChild;
		}
		else if (node == node->_pParent->_pLeft)
		{
			node->_pParent->_pLeft = leftChild;
		}
		else
		{
			node->_pParent->_pRight = leftChild;
		}

		node->_pParent = leftChild;
		leftChild->_pRight = node;
	}


	void RedBlackTree::GetArrayInternal(int* const outArray, const size_t len, const Node* const curNode, size_t* const index) const
	{
		if (curNode == _nil)
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

	void RedBlackTree::PrintTreeInternal(const Node* const curNode, int& order, const int depth) const
	{
		if (curNode == _nil)
		{
			return;
		}

		PrintTreeInternal(curNode->_pLeft, order, depth + 1);

		ConsoleManager::SetCursorPosition(order * 3, depth);
		if (curNode->_color == eNodeColor::Black)
		{
			ConsoleManager::SetCursorColor(ConsoleManager::eConsoleColor::BLUE);
		}
		else
		{
			ConsoleManager::SetCursorColor(ConsoleManager::eConsoleColor::RED);
		}

		wprintf(L"%d", curNode->_val);
		order++;

		ConsoleManager::SetCursorColor(ConsoleManager::eConsoleColor::WHITE);

		PrintTreeInternal(curNode->_pRight, order, depth + 1);

		return;
	}

	void RedBlackTree::Terminate(const Node* const curNode) const
	{
		if (curNode == _nil)
		{
			return;
		}

		Terminate(curNode->_pLeft);
		Terminate(curNode->_pRight);

		delete curNode;

		return;
	}
}