#include "RedBlackTree.h"
#include "ConsoleManager.h"

#include <cstdio>
#include <cassert>
#include <cstdlib>

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
			
			if (targetNode->_pRight != _nil)
			{
				targetNode->_pRight->_pParent = targetParentNode;
			}

			if (targetNode->_color == eNodeColor::Black)
			{
				RebalanceDelete(targetNode->_pRight, targetParentNode);
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

			targetNode->_pLeft->_pParent = targetParentNode;

			if (targetNode->_color == eNodeColor::Black)
			{
				RebalanceDelete(targetNode->_pLeft, targetParentNode);
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
				nextParentNode->_pRight = nextNode->_pRight;

				if (nextNode->_pRight != _nil)
				{
					nextNode->_pRight->_pParent = nextParentNode;
				}
			}
			else
			{
				nextParentNode->_pLeft = nextNode->_pRight;

				if (nextNode->_pRight != _nil)
				{
					nextNode->_pRight->_pParent = nextParentNode;
				}
			}

			if (nextNode->_color == eNodeColor::Black)
			{
				RebalanceDelete(nextNode->_pRight, nextParentNode);
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
		system("cls");

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

	void RedBlackTree::RebalanceInsert(Node* const node)
	{
		Node* curNode = node;

		while (curNode->_pParent->_color == eNodeColor::Red)
		{
			if (curNode->_pParent == curNode->_pParent->_pParent->_pLeft)
			{
				Node* uncle = curNode->_pParent->_pParent->_pRight;
				if (uncle->_color == eNodeColor::Red)
				{
					uncle->_color = eNodeColor::Black;
					curNode->_pParent->_color = eNodeColor::Black;
					curNode->_pParent->_pParent->_color = eNodeColor::Red;
					curNode = curNode->_pParent->_pParent;
				}
				else
				{
					if (curNode == curNode->_pParent->_pRight)
					{
						curNode = curNode->_pParent;
						LeftSpin(curNode);
					}

					curNode->_pParent->_color = eNodeColor::Black;
					curNode->_pParent->_pParent->_color = eNodeColor::Red;
					RightSpin(curNode->_pParent->_pParent);
				}
			}
			else
			{
				Node* uncle = curNode->_pParent->_pParent->_pLeft;
				if (uncle->_color == eNodeColor::Red)
				{
					uncle->_color = eNodeColor::Black;
					curNode->_pParent->_color = eNodeColor::Black;
					curNode->_pParent->_pParent->_color = eNodeColor::Red;
					curNode = curNode->_pParent->_pParent;
				}
				else
				{
					if (curNode == curNode->_pParent->_pLeft)
					{
						curNode = curNode->_pParent;
						RightSpin(curNode);
					}

					curNode->_pParent->_color = eNodeColor::Black;
					curNode->_pParent->_pParent->_color = eNodeColor::Red;
					LeftSpin(curNode->_pParent->_pParent);
				}
			}
		}

		_root->_color = eNodeColor::Black;
	}

	void RedBlackTree::RebalanceDelete(Node* const node, Node* const parentNode)
	{
		Node* curNode = node;
		Node* curParentNode = parentNode;

		while (curNode != _root && curNode->_color == eNodeColor::Black)
		{
			if (curNode == curParentNode->_pLeft)
			{
				Node* sibling = curParentNode->_pRight;

				if (sibling->_color == eNodeColor::Red)
				{
					sibling->_color = eNodeColor::Black;
					curParentNode->_color = eNodeColor::Red;
					LeftSpin(curParentNode);
					sibling = curParentNode->_pRight;
				}

				if (sibling->_pLeft->_color == eNodeColor::Black && sibling->_pRight->_color == eNodeColor::Black)
				{
					sibling->_color = eNodeColor::Red;
					curNode = curParentNode;
					curParentNode = curNode->_pParent;
					continue;
				}
				else
				{
					if (sibling->_pLeft->_color == eNodeColor::Red && sibling->_pRight->_color == eNodeColor::Black)
					{
						sibling->_color = eNodeColor::Red;
						sibling->_pLeft->_color = eNodeColor::Black;
						RightSpin(sibling);

						sibling = curParentNode->_pRight;
					}

					sibling->_color = curParentNode->_color;
					curParentNode->_color = eNodeColor::Black;
					sibling->_pRight->_color = eNodeColor::Black;
					LeftSpin(curParentNode);

					break;
				}
			}
			else
			{
				Node* sibling = curParentNode->_pLeft;

				if (sibling->_color == eNodeColor::Red)
				{
					sibling->_color = eNodeColor::Black;
					curParentNode->_color = eNodeColor::Red;
					RightSpin(curParentNode);
					sibling = curParentNode->_pLeft;
				}

				if (sibling->_pLeft->_color == eNodeColor::Black && sibling->_pRight->_color == eNodeColor::Black)
				{
					sibling->_color = eNodeColor::Red;
					curNode = curParentNode;
					curParentNode = curNode->_pParent;
					continue;
				}
				else
				{
					if (sibling->_pRight->_color == eNodeColor::Red && sibling->_pLeft->_color == eNodeColor::Black)
					{
						sibling->_color = eNodeColor::Red;
						sibling->_pRight->_color = eNodeColor::Black;
						LeftSpin(sibling);

						sibling = curParentNode->_pLeft;
					}

					sibling->_color = curParentNode->_color;
					curParentNode->_color = eNodeColor::Black;
					sibling->_pLeft->_color = eNodeColor::Black;
					RightSpin(curParentNode);

					break;
				}
			}
		}

		curNode->_color = eNodeColor::Black;
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

		ConsoleManager::SetCursorPosition(order * 3, depth * 2);
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

	bool RedBlackTree::VerifyTree(void)
	{
		int maxNumOfBlack = -1;

		return VerifyTreeInternal(_root, maxNumOfBlack, 0);
	}

	bool RedBlackTree::VerifyTreeInternal(const Node* const node, int& maxBlackNum, int curBlackNum)
	{
		if (node->_color == eNodeColor::Black)
		{
			curBlackNum++;
		}

		if (node == _nil)
		{
			if (maxBlackNum == -1)
			{
				maxBlackNum = curBlackNum;
				return true;
			}
			else if (maxBlackNum == curBlackNum)
			{
				return true;
			}

			return false;
		}

		bool result = true;

		result &= VerifyTreeInternal(node->_pLeft, maxBlackNum, curBlackNum);
		result &= VerifyTreeInternal(node->_pRight, maxBlackNum, curBlackNum);

		return result;
	}
}