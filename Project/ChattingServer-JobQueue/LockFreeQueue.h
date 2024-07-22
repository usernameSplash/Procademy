#pragma once

#include "ObjectPool.h"

template<typename T>
class LockFreeQueue
{
public:
	struct Node
	{
		T _data;
		__int64 _next;
	};

public:
	LockFreeQueue();
	~LockFreeQueue();

public:
	void Enqueue(T data);
	T Dequeue(void);

	__int64 Size(void) const;
public:
	__int64 _head;
	__int64 _tail;
	__int64 _key;
	__int64 _size;

public:
	__int64 _queueId;
	static __int64 _queueIdProvider;
	static ObjectPool<Node>* _queueNodePool;
};


template<typename T>
LockFreeQueue<T>::LockFreeQueue()
	: _key(0)
	, _size(0)
{
	_queueId = InterlockedIncrement64(&_queueIdProvider);

	Node* dummyNode = _queueNodePool->Alloc();
	dummyNode->_next = (_queueId << POOL_KEY_BITMASK_64BIT) & KEY_MASK;

	_head = (__int64)dummyNode;
	_tail = (__int64)dummyNode;
	_size = 0;
}

template<typename T>
LockFreeQueue<T>::~LockFreeQueue()
{

}

template<typename T>
void LockFreeQueue<T>::Enqueue(T data)
{
	__int64 key;
	key = InterlockedIncrement64(&_key);

	Node* newTailNode;
	newTailNode = _queueNodePool->Alloc();
	newTailNode->_data = data;
	
	__int64 newTail;
	newTail = GET_PTR((__int64)newTailNode);
	newTail = ((key << POOL_KEY_BITMASK_64BIT) & KEY_MASK) | newTail;

	while (true)
	{
		__int64 tempTail;
		Node* tempTailNode;
		__int64 next;

		tempTail = _tail;
		tempTailNode = (Node*)GET_PTR(tempTail);
		next = tempTailNode->_next;

		if (GET_PTR(next) == NULL)
		{
			if (InterlockedCompareExchange64(&tempTailNode->_next, newTail, next) == next)
			{
				InterlockedCompareExchange64(&_tail, newTail, tempTail);
				InterlockedIncrement64(&_size);

				break;
			}
		}
		else
		{
			InterlockedCompareExchange64(&_tail, next, tempTail);
		}
	}
}

template<typename T>
T LockFreeQueue<T>::Dequeue(void)
{
	T data;

	while (true)
	{
		__int64 tail;
		__int64 head;
		__int64 next;
		Node* headNode;

		head = _head;
		tail = _tail;
		headNode = (Node*)GET_PTR(head);
		next = headNode->_next;

		if (_size == 0)
		{
			return 0;
		}

		if (GET_PTR(next) == NULL)
		{
			return 0;
		}

		if (head == tail)
		{
			InterlockedCompareExchange64(&_tail, next, tail);
			continue;
		}

		Node* nextNode;
		nextNode = (Node*)GET_PTR(next);
		data = nextNode->_data;

		if (InterlockedCompareExchange64(&_head, next, head) == head)
		{
			headNode->_data = 0;
			headNode->_next = NULL;
			_queueNodePool->Free(headNode);
			InterlockedDecrement64(&_size);

			break;
		}
	}


	return data;
}

template<typename T>
__int64 LockFreeQueue<T>::Size(void) const
{
	return _size;
}

template<typename T>
__int64 LockFreeQueue<T>::_queueIdProvider = 0;


template<typename T>
ObjectPool<typename LockFreeQueue<T>::Node>* LockFreeQueue<T>::_queueNodePool = new ObjectPool<LockFreeQueue<T>::Node>(20000);