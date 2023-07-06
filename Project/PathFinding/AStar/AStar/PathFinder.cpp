#include "PathFinder.h"

namespace PathFinder
{
	PathFinder::PathFinder(Map* map)
		: _bPathFound(false)
		, _bPathFinding(false)
		, _map(map)
		, _startNode(nullptr)
		, _destNode(nullptr)
		, _openList(4096)
	{
		_closeList.reserve(4096);
	}

	PathFinder::~PathFinder()
	{
		DeallocAllNodes();
	}

	void PathFinder::DeallocAllNodes(void)
	{
		_openList.Clear();

		for (auto it = _closeList.begin(); it != _closeList.end(); ++it)
		{
			delete (*it);
		}

		_closeList.clear();
		//delete _destNode;

		_startNode = nullptr;
		_destNode = nullptr;
	}

	bool PathFinder::IsPathFound(void) const 
	{
		return _bPathFound;
	}

	bool PathFinder::IsPathFinding(void) const
	{
		return _bPathFinding;
	}

	bool PathFinder::IsTriedLastTime(void) const
	{
		return _bTriedLastTime;
	}

	void PathFinder::StartPathFinding(void)
	{
		if (_bPathFinding)
		{
			return;
		}

		if (_openList.empty() == false || _closeList.empty() == false)
		{
			DeallocAllNodes(); // if previous result is left, delete that.
		}
		
		_map->ResetMap();

		_startNode = new Node(_map->_startPos, 0, _map->_startPos.DistanceManhattan(_map->_destPos), Dir::NONE, nullptr);
		_openList.push(_startNode);
		_bPathFinding = true;
		_bPathFound = false;
	}

	void PathFinder::EndPathFinding(void)
	{
		_bPathFinding = false;
	}

	void PathFinder::SetMap(Map* map)
	{
		_map = map;
	}

	Node* PathFinder::GetStartNode(void) const
	{
		return _startNode;
	}

	Node* PathFinder::GetDestNode(void) const
	{
		return _destNode;
	}

	const NodeHeap& PathFinder::GetOpenList(void) const
	{
		return _openList;
	}

	const std::vector<Node*> PathFinder::GetCloseList(void) const
	{
		return _closeList;
	}

	const std::vector<Vector2>& PathFinder::GetCornerList(void) const
	{
		return _cornerList;
	}
}