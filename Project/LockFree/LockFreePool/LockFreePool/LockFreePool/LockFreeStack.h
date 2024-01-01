#pragma once

#include "LockFreePool.h"
#include "LogData.h"

#include <Windows.h>
#include <process.h>


template<typename T>
class LockFreeStack
{
private:
	struct Node
	{
		T _value;
		__int64 _next;
	};

public:
	LockFreeStack()
		: _top(NULL)
		, _key(-1)
	{
		_pool = new LockFreePool<Node>(10000);
	}

	~LockFreeStack()
	{
		Node* curNode = (Node*)GET_PTR(_top);
		while (curNode != NULL)
		{
			Node* nextNode = (Node*)GET_PTR(curNode->_next);
			
			curNode = nextNode;
		}
	}

	void Push(T data);
	T Pop(void);

public:
	__int64 _top;
private:
	LockFreePool<Node>* _pool;
	__int64 _key;
};

template<typename T>
void LockFreeStack<T>::Push(T data)
{
	__int64 tempTop;
	__int64 newTop;
	__int64 key;
	Node* newNode;

	key = InterlockedIncrement64(&_key);

	newNode = _pool->Alloc();
	newNode->_value = data;

	newTop = GET_PTR((__int64)newNode);
	newTop = ((key << POOL_KEY_BITMASK_64BIT) | newTop);

#ifdef __LOCK_FREE_DEBUG__
	__int64 idx;
	idx = InterlockedIncrement64(&g_logIndex);
	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_PUSH;
	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = 0;
	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = 0;
#endif

	while (true)
	{
		tempTop = _top;
		newNode->_next = tempTop;

		if (InterlockedCompareExchange64(&_top, newTop, tempTop) == tempTop)
		{
#ifdef __LOCK_FREE_DEBUG__
			idx = InterlockedIncrement64(&g_logIndex);
#endif
			InterlockedIncrement64(&g_pushCnt);
			break;
		}
	}

#ifdef __LOCK_FREE_DEBUG__
	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_PUSH;
	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = (void*)newTop;
	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = (void*)tempTop;
#endif

	if (GET_PTR(tempTop) == GET_PTR(newTop))
	{
		__debugbreak();
	}
}

template<typename T>
T LockFreeStack<T>::Pop(void)
{
	__int64 tempTop;
	__int64 next;
	Node* curNode;
	T data;
	
#ifdef __LOCK_FREE_DEBUG__
	__int64 idx;
	idx = InterlockedIncrement64(&g_logIndex);
	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_POP;
	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = 0;
	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = 0;
#endif 

	while (true)
	{
		tempTop = _top;
		curNode = (Node*)GET_PTR(tempTop);
		next = curNode->_next;

		if (InterlockedCompareExchange64(&_top, next, tempTop) == tempTop)
		{
#ifdef __LOCK_FREE_DEBUG__
			idx = InterlockedIncrement64(&g_logIndex);
#endif
			InterlockedIncrement64(&g_popCnt);
			break;
		}
	}

	data = curNode->_value;
	curNode->_next = 0;
	_pool->Free(curNode);
	
#ifdef __LOCK_FREE_DEBUG__
	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_POP;
	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = (void*)tempTop;
	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = (void*)next;
#endif

	if (GET_PTR(tempTop) == GET_PTR(next))
	{
		__debugbreak();
	}

	if (data != 1)
	{
		int* p = nullptr;
		*p = 1;
	}

	return data;
}