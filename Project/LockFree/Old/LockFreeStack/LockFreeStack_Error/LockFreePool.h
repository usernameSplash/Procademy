#pragma once

#pragma once

template<typename T>
class LockFreePool
{
private:
	struct Node
	{
		T _data;
		Node* _next;
	};

public:
	LockFreePool()
		: _top(nullptr)
	{
	}

	~LockFreeStack()
	{
		Node* curNode = _top;
		while (curNode != nullptr)
		{
			Node* deleteNode = curNode;
			delete deleteNode;

			curNode = curNode->_next;
		}
	}

public:
	void Free(T* data)
	{
		Node* node = static_cast<Node*>(data);
		Node* top;

		do
		{
			top = _top;
			node->_next = _top;
		} while (InterlockedCompareExchange64((LONG64*)&_top, (LONG64)node, (LONG64)top) != top);
	}

	T* Alloc(void)
	{
		T data;
		Node* top;
		Node* next;

		do
		{
			top = _top;
			data = top->_data;
			next = top->_next;
		} while (InterlockedCompareExchange64((LONG64*)&_top, (LONG64)next, (LONG64)top) != top);

		return data;
	}

private:
	Node* _top;

	/*
		For Debug
	*/

private:
#define DEBUG_ARRAY_LEN 10000
	struct DebugData
	{
		DebugData()
			: _idx(-1)
			, _threadId(-1)
			, _operationCode(-1)
			, _pNewTopNode(nullptr)
			, _pOldTopNode(nullptr)
			, _newTopVal(-1)
			, _oldTopVal(-1)
		{
		}

		void SetData(int idx, int threadId, int operationCode, Node* pNewTopNode, Node* pOldTopNode, T newTopVal, T oldTopVal)
		{
			_idx = idx;
			_threadId = threadId;
			_operationCode = operationCode;
			_pNewTopNode = pNewTopNode;
			_pOldTopNode = pOldTopNode;
			_newTopVal = newTopVal;
			_oldTopVal = oldTopVal;
		}

		int _idx;
		int _threadId;
		int _operationCode; //0 == Push, 1 == Pop;
		Node* _pNewTopNode;
		Node* _pOldTopNode;
		T _newTopVal;
		T _oldTopVal;
	};

private:
	void LogData(int operationCode, Node* pNewTopNode, Node* pOldTopNode, T newTopVal, T oldTopVal)
	{
		long idx = InterlockedIncrement(&_idx);
		int threadId = GetCurrentThreadId();

		_debugArray[idx % DEBUG_ARRAY_LEN].SetData(idx, threadId, operationCode, pNewTopNode, pOldTopNode, newTopVal, oldTopVal);


	}

private:
	long _debugIdx;
	DebugData _debugArray[DEBUG_ARRAY_LEN];
};