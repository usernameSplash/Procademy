#pragma once

#include "LockFreePool.h"

template<typename T>
class LockFreeQueue
{
private:
	struct Node
	{
		T _value;
		__int64 _next;
	};
public:
	LockFreeQueue(int poolSize)
		: _key(0)
		, _size(0)
	{
		_pool = new LockFreePool<Node>(poolSize * 2);

		Node* dummyNode = _pool->Alloc();
		dummyNode->_next = NULL;

		__int64 dummy = (__int64)dummyNode;

		_head = dummy;
		_tail = dummy;

	}
	~LockFreeQueue()
	{
		Node* curNode = (Node*)GET_PTR(_head);
		while (curNode != NULL)
		{
			Node* nextNode = (Node*)GET_PTR(curNode->_next);
			_pool->Free(curNode);
			curNode = nextNode;
		}
	}

	void Enqueue(T data);
	T Dequeue(void);

public:
	__int64 _head;
	__int64 _tail;
	__int64 _size;

private:
	LockFreePool<Node>* _pool;
	__int64 _key;
};

template<typename T>
void LockFreeQueue<T>::Enqueue(T data)
{
	__int64 prevTail;
	__int64 prevTailNext;
	__int64 newTail;
	__int64 key;

	Node* prevTailNode;
	Node* newTailNode;

	__int64 cas2Ret;
	__int64 cas3Ret;

	__int64 size;

	key = InterlockedIncrement64(&_key);
	newTailNode = _pool->Alloc();
	newTailNode->_value = data;
	newTailNode->_next = NULL;

	newTail = GET_PTR(reinterpret_cast<__int64>(newTailNode));
	newTail = ((key << POOL_KEY_BITMASK_64BIT) | newTail);

	size = _size;
	Log(JOB_ENQ_BEFORE_LOOP, newTail, 0, 0, _size);
	while (true)
	{
		size = _size;
		prevTail = _tail;
		prevTailNode = (Node*)GET_PTR(prevTail);

		prevTailNext = prevTailNode->_next;

		if (prevTailNext == NULL)
		{
			if (InterlockedCompareExchange64(&prevTailNode->_next, newTail, prevTailNext) == prevTailNext)
			{
				Log(JOB_ENQ_CAS1, newTail, prevTailNext, 0, size);

				cas2Ret = InterlockedCompareExchange64(&_tail, newTail, prevTail);

				if (cas2Ret != prevTail)
				{
					Log(JOB_ENQ_CAS2_FAIL, newTail, prevTail, cas2Ret, size);
				}
				else
				{
					Log(JOB_ENQ_CAS2, newTail, prevTail, cas2Ret, size);
				}
				break;
			}
		}
		else
		{
			//cas3Ret = InterlockedCompareExchange64(&_tail, prevTailNext, prevTail);

			//if (cas3Ret != prevTail)
			//{
			//	Log(JOB_ENQ_CAS3_FAIL, prevTailNext, prevTail, cas3Ret);
			//}
			//else
			//{
			//	Log(JOB_ENQ_CAS3, prevTailNext, prevTail, cas3Ret);
			//}
		}

	}

	InterlockedIncrement64(&_size);
}

template<typename T>
T LockFreeQueue<T>::Dequeue(void)
{
	__int64 prevHead;
	__int64 prevHeadNext;
	__int64 prevTail;

	Node* prevHeadNode;
	Node* prevHeadNextNode;

	__int64 casRet;

	T data {};
	__int64 size;

	if (_size == 0)
	{
		return data;
	}

	size = _size;
	Log(JOB_DEQ_BEFORE_LOOP, 0, 0, 0, size);
	while (true)
	{
		size = _size;
		prevTail = _tail;
		prevHead = _head;
		prevHeadNode = (Node*)GET_PTR(prevHead);

		prevHeadNext = prevHeadNode->_next;
		prevHeadNextNode = (Node*)GET_PTR(prevHeadNext);

		if (prevHeadNext == NULL)
		{
			Log(JOB_DEQ_NEXT_NULL, NULL, NULL, prevHead, size);
			return data;
		}

		casRet = InterlockedCompareExchange64(&_head, prevHeadNext, prevHead);
		if (casRet == prevHead)
		{
			Log(JOB_DEQ_CAS, prevHeadNext, prevHead, casRet, size);
			data = prevHeadNextNode->_value;
			prevHeadNode->_next = 0xfdfdfdfdfdfdfdfd;
			_pool->Free(prevHeadNode);
			break;
		}
	}

	InterlockedDecrement64(&_size);

	return data;
}
