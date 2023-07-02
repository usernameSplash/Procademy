#include "NodeHeap.h"

#include <compare>
#include <cmath>

namespace PathFinder
{
	void Node::Set(Node* parentNode, Node* destNode, Dir dir)
	{
		switch (dir)
		{
		case Dir::LL:
			{
				_x = parentNode->_x - 1;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::UU:
			{
				_y = parentNode->_y - 1;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::RR:
			{
				_x = parentNode->_x + 1;
				_g = parentNode->_g + COMMON;
				break;
			}
		case Dir::DD:
			{
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
		default:
			break;
		}

		_h = abs(destNode->_x - _x) + abs(destNode->_y - _y);
		_f = _g + _h;

		_from = parentNode;
	}

	NodeHeap::NodeHeap(const size_t size = 4096)
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