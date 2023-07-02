#include "NodeHeap.h"

#include <compare>
#include <cmath>

namespace PathFinder
{
	void NodeHeap::Node::Set(Node* parentNode, Node* endNode, Dir dir)
	{
		int dist = abs(parentNode->_x - _x);
		if (dist == 0)
		{
			dist = abs(parentNode->_y - _y);
		}

		switch (dir)
		{
		case Dir::LL:
			{
				_x = parentNode->_x - 1;
				_g = parentNode->_g + COMMON * dist;
				break;
			}
		case Dir::UU:
			{
				_y = parentNode->_y - 1;
				_g = parentNode->_g + COMMON * dist;
				break;
			}
		case Dir::RR:
			{
				_x = parentNode->_x + 1;
				_g = parentNode->_g + COMMON * dist;
				break;
			}
		case Dir::DD:
			{
				_y = parentNode->_y + 1;
				_g = parentNode->_g + COMMON * dist;
				break;
			}
		case Dir::LU:
			{
				_x = parentNode->_x - 1;
				_y = parentNode->_y - 1;
				_g = parentNode->_g + DIAGNAL * dist;
				break;
			}
		case Dir::RU:
			{
				_x = parentNode->_x + 1;
				_y = parentNode->_y - 1;
				_g = parentNode->_g + DIAGNAL * dist;
				break;
			}
		case Dir::RD:
			{
				_x = parentNode->_x + 1;
				_y = parentNode->_y + 1;
				_g = parentNode->_g + DIAGNAL * dist;
				break;
			}
		case Dir::LD:
			{
				_x = parentNode->_x - 1;
				_y = parentNode->_y + 1;
				_g = parentNode->_g + DIAGNAL * dist;
				break;
			}
		default:
			break;
		}

		_h = abs(endNode->_x - _x) + abs(endNode->_y - _y);

		_f = _g + _h;
	}

	NodeHeap::NodeHeap()
	{
		std::vector<Node> container;
		container.reserve(4096);

		_heap = new std::priority_queue<Node, std::vector<Node>>(std::less<Node>(), std::move(container));
	}
}