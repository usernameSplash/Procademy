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
private:
	__int64 _head;
	__int64 _tail;
	__int64 _key;
	__int64 _size;

public:
	static ObjectPool<Node>* _queueNodePool;
};


template<typename T>
LockFreeQueue<T>::LockFreeQueue()
	: _key(0)
	, _size(0)
{
	Node* dummyNode = _queueNodePool->Alloc();
	dummyNode->_next = NULL;

	_head = (__int64)dummyNode;
	_tail = (__int64)dummyNode;
}

template<typename T>
LockFreeQueue<T>::~LockFreeQueue()
{

}

template<typename T>
void LockFreeQueue<T>::Enqueue(T data)
{
	__int64 tempTail;
	__int64 newTail;
	__int64 key;
	Node* tempTailNode;
	Node* newTailNode;

	key = InterlockedIncrement64(&key);

	newTailNode = _queueNodePool->Alloc();
	newTailNode->_data = data;

	newTail = GET_PTR((__int64)newTailNode);
	newTail = ((key << POOL_KEY_BITMASK_64BIT) | newTail);

	while (true)
	{
		__int64 next;

		tempTail = _tail;
		tempTailNode = (Node*)GET_PTR(tempTail);
		next = tempTailNode->_next;

		if (next == NULL)
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
	__int64 tail;
	__int64 head;
	Node* headNode;
	T data;

	while (true)
	{
		__int64 next;
		Node* nextNode;

		head = _head;
		tail = _tail;
		headNode = (Node*)GET_PTR(head);
		next = headNode->_next;

		if (_size == 0)
		{
			return 0;
		}

		if (next == NULL)
		{
			return 0;
		}

		if (head == tail)
		{
			InterlockedCompareExchange64(&_tail, next, tail);
			continue;
		}

		nextNode = (Node*)GET_PTR(next);
		data = nextNode->_data;

		if (InterlockedCompareExchange64(&_head, next, head) == head)
		{
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
ObjectPool<typename LockFreeQueue<T>::Node>* LockFreeQueue<T>::_queueNodePool = new ObjectPool<LockFreeQueue<T>::Node>(20000);