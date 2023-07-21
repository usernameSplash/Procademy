#include "JPS.h"

namespace PathFinder
{
	JPS::JPS(Map* map)
		: PathFinder(map)
	{
	}

	JPS::~JPS()
	{
	}

	void JPS::PathFind(void)
	{
		if (_openList.empty())
		{
			_bPathFound = false;
			goto FAILED_PATH_FINDING;
		}

		Node* curNode;
		int index;

		while (true)
		{
			curNode = _openList.top();
			_openList.pop();
			index = curNode->_pos._y * _map->_width + curNode->_pos._x;

			if (_map->_grid[index] != eGridStatus::VISITED)
			{
				break;
			}
			else if (_openList.empty())
			{
				goto FAILED_PATH_FINDING;
			}
		}

		if (_map->_grid[index] == eGridStatus::DEST)
		{
			_bPathFound = true;
			_closeList.push_back(curNode);
			_destNode = curNode;
			goto FINISHED_PATH_FINDING;
		}

		if (_map->_grid[index] == eGridStatus::SEARCHED)
		{
			_map->_grid[index] = eGridStatus::VISITED;
			_closeList.push_back(curNode);
		}

		ResetCornerList();
		CreateNode(curNode);

		return;


	FAILED_PATH_FINDING:
		_bPathFound = false;
	FINISHED_PATH_FINDING:
		_bPathFinding = false;
		_bTriedLastTime = true;
		return;
	}

	void JPS::CreateNode(Node* node)
	{
		Vector2 cornerPos { 0,0 };
		Dir cornerDir = Dir::NONE;
		Dir cornerSearchDir = Dir::NONE;

		int newG = node->_g;

		switch (node->_dir)
		{
		case Dir::LL:
			if (FindCorner(node, Dir::LL, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LL, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			

		case Dir::UU:
			if (FindCorner(node, Dir::UU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::UU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			
			break;

		case Dir::RR:

			if (FindCorner(node, Dir::RR, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RR, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			break;

		case Dir::DD:
			if (FindCorner(node, Dir::DD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::DD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;

		case Dir::LU:
			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LL, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LL, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::UU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::UU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;
		case Dir::RU:
			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RR, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RR, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::UU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::UU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;
		case Dir::RD:
			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RR, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RR, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::DD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::DD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;
		case Dir::LD:
			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LL, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LL, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::DD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::DD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;
		case Dir::NONE:
			if (FindCorner(node, Dir::LL, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LL, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED); 
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::UU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::UU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RR, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RR, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED); 
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::DD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceManhattan(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::DD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RU, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RU, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::RD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::RD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}

			if (FindCorner(node, Dir::LD, cornerPos))
			{
				Node* newNode = new Node(cornerPos, newG + cornerPos.DistanceDiagnal(node->_pos), cornerPos.DistanceManhattan(_map->_destPos), Dir::LD, node);
				_map->SetValue(cornerPos, eGridStatus::SEARCHED);
				_openList.push(newNode);
			}
			break;
		default:
			break;
		}
	}

	bool JPS::FindCorner(Node* node, const Dir searchDir, Vector2& outCornerPos)
	{
		Vector2 newPos = node->_pos;

		switch (searchDir)
		{
		case Dir::LL:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::LL)];

				while (newPos._x >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					switch (status)
					{
					case eGridStatus::BLOCKED:
						/* intentional fallthrough */
					case eGridStatus::SEARCHED:
						/* intentional fallthrough */
					case eGridStatus::START:
						/* intentional fallthrough */
					case eGridStatus::VISITED:
						/* intentional fallthrough */
					case eGridStatus::INVALID:
						return false;
					case eGridStatus::DEST:
						outCornerPos = newPos;
						return true;
					default:
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::LL)];
				}
				break;
			}
		case Dir::UU:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::UU)];

				while (newPos._y >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					switch (status)
					{
					case eGridStatus::BLOCKED:
						/* intentional fallthrough */
					case eGridStatus::SEARCHED:
						/* intentional fallthrough */
					case eGridStatus::START:
						/* intentional fallthrough */
					case eGridStatus::VISITED:
						/* intentional fallthrough */
					case eGridStatus::INVALID:
						return false;
					case eGridStatus::DEST:
						outCornerPos = newPos;
						return true;
					default:
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::UU)];
				}
				break;
			}
		case Dir::RR:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::RR)];

				while (newPos._x < _map->_width)
				{
					eGridStatus status = _map->GetValue(newPos);

					switch (status)
					{
					case eGridStatus::BLOCKED:
						/* intentional fallthrough */
					case eGridStatus::SEARCHED:
						/* intentional fallthrough */
					case eGridStatus::START:
						/* intentional fallthrough */
					case eGridStatus::VISITED:
						/* intentional fallthrough */
					case eGridStatus::INVALID:
						return false;
					case eGridStatus::DEST:
						outCornerPos = newPos;
						return true;
					default:
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::RR)];
				}
				break;
			}
		case Dir::DD:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::DD)];

				while (newPos._y < _map->_depth)
				{
					eGridStatus status = _map->GetValue(newPos);

					switch (status)
					{
					case eGridStatus::BLOCKED:
						/* intentional fallthrough */
					case eGridStatus::SEARCHED:
						/* intentional fallthrough */
					case eGridStatus::START:
						/* intentional fallthrough */
					case eGridStatus::VISITED:
						/* intentional fallthrough */
					case eGridStatus::INVALID:
						return false;
					case eGridStatus::DEST:
						outCornerPos = newPos;
						return true;
					default:
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::DD)];
				}
				break;
			}
		case Dir::LU:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::LU)];

				while (newPos._x >= 0 && newPos._y >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					if (status == eGridStatus::DEST)
					{
						outCornerPos = newPos;
						return true;
					}
					else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
					{
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					Vector2 diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::LL)];
					while (diagnalCheckingPos._x >= 0)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
								_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)];
					}

					diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::UU)];
					while (diagnalCheckingPos._y >= 0)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)];
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::LU)];
				}
				break;
			}
		case Dir::RU:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::RU)];

				while (newPos._x < _map->_width && newPos._y >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					if (status == eGridStatus::DEST)
					{
						outCornerPos = newPos;
						return true;
					}
					else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
					{
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					Vector2 diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::RR)];
					while (diagnalCheckingPos._x < _map->_width)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)];
					}

					diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::UU)];
					while (diagnalCheckingPos._y >= 0)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)];
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::RU)];
				}
				break;
			}
		case Dir::RD:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::RD)];

				while (newPos._x < _map->_width && newPos._y >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					if (status == eGridStatus::DEST)
					{
						outCornerPos = newPos;
						return true;
					}
					else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
					{
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					Vector2 diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::RR)];
					while (diagnalCheckingPos._x < _map->_width)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
								_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)];
					}

					diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::DD)];
					while (diagnalCheckingPos._y < _map->_depth)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
								_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)];
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::RD)];
				}
				break;
			}
		case Dir::LD:
			{
				newPos = newPos + _deltaPos[static_cast<int>(Dir::LD)];

				while (newPos._x < _map->_width && newPos._y >= 0)
				{
					eGridStatus status = _map->GetValue(newPos);

					if (status == eGridStatus::DEST)
					{
						outCornerPos = newPos;
						return true;
					}
					else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
					{
						break;
					}

					if ((_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
						_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
						(_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
							_map->GetValue(newPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
					{
						outCornerPos = newPos;
						return true;
					}

					Vector2 diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::LL)];
					while (diagnalCheckingPos._x >= 0)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LU)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::UU)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
								_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)];
					}

					diagnalCheckingPos = newPos + _deltaPos[static_cast<int>(Dir::DD)];
					while (diagnalCheckingPos._y < _map->_depth)
					{
						eGridStatus status = _map->GetValue(diagnalCheckingPos);

						if (status == eGridStatus::DEST)
						{
							outCornerPos = newPos;
							return true;
						}
						else if (status == eGridStatus::BLOCKED || status == eGridStatus::SEARCHED || status == eGridStatus::START || status == eGridStatus::VISITED || status == eGridStatus::INVALID)
						{
							break;
						}

						if ((_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LD)]) != eGridStatus::BLOCKED &&
							_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::LL)]) == eGridStatus::BLOCKED) ||
							(_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RD)]) != eGridStatus::BLOCKED &&
								_map->GetValue(diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::RR)]) == eGridStatus::BLOCKED))
						{
							AddCorner(diagnalCheckingPos);
							outCornerPos = newPos;
							return true;
						}

						diagnalCheckingPos = diagnalCheckingPos + _deltaPos[static_cast<int>(Dir::DD)];
					}

					newPos = newPos + _deltaPos[static_cast<int>(Dir::LD)];
				}
				break;
			}
		default:
			break;
		}

		return false;
	}

	void JPS::AddCorner(Vector2 pos)
	{
		_cornerList.push_back(pos);
	}

	void JPS::ResetCornerList(void)
	{
		_cornerList.clear();
	}
}