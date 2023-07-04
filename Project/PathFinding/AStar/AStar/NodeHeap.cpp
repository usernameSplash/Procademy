#include "NodeHeap.h"

#include <cmath>

namespace PathFinder
{
	void Node::SetPos(const int x, const int y)
	{
		_x = x;
		_y = y;
	}

	void Node::Set(Node* parentNode, const Node* destNode, const Dir dir)
	{
		switch (dir)
		{
		case Dir::LL:
			{
				_x = parentNode->_x - 1;
				_y = parentNode->_y;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::UU:
			{
				_x = parentNode->_x;
				_y = parentNode->_y - 1;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::RR:
			{
				_x = parentNode->_x + 1;
				_y = parentNode->_y;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::DD:
			{
				_x = parentNode->_x;
				_y = parentNode->_y + 1;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::LU:
			{
				_x = parentNode->_x - 1;
				_y = parentNode->_y - 1;
				_g = parentNode->_g + DIAGNAL;
				break;
			}
		case Dir::RU:
			{
				_x = parentNode->_x + 1;
				_y = parentNode->_y - 1;
				_g = parentNode->_g + DIAGNAL;
				break;
			}
		case Dir::RD:
			{
				_x = parentNode->_x + 1;
				_y = parentNode->_y + 1;
				_g = parentNode->_g + DIAGNAL;
				break;
			}
		case Dir::LD:
			{
				_x = parentNode->_x - 1;
				_y = parentNode->_y + 1;
				_g = parentNode->_g + DIAGNAL;
				break;
			}
		case Dir::NONE:
			{
				break; // startNode;
			}
		default:
			break;
		}

		_h = (abs(destNode->_x - _x) + abs(destNode->_y - _y)) * 100;
		_f = _g + _h;

		_from = parentNode;
	}

	NodeHeap::NodeHeap(const size_t size)
	{
		c.reserve(size);
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