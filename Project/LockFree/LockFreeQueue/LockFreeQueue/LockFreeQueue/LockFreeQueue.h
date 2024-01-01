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
	LockFreeQueue()
		: _top(NULL)
		, _key(-1)
	{
		_pool = new LockFreePool<Node>(10000);
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
	__int64 newTail;
	__int64 key;
	Node* prevTailNode;
	Node* newTailNode;

	key = InterlockedIncrement64(&_key);
	newTailNode = _pool->Alloc();
	newTailNode->_value = data;
	newTailNode->_next = NULL;

	newTail = GET_PTR(reinterpret_cast<__int64>(newTailNode));
	newTail = ((key << POOL_KEY_BITMASK_64BIT) | newTail);

	while (true)
	{
		prevTail = _tail;
		prevTailNode = (Node*)GET_PTR(prevTail);

		if(InterlockedCompareExchange64(prevTailNode->_next, ))
	}

	InterlockedIncrement64(&_size);
}

template<typename T>
T LockFreeQueue<T>::Dequeue(void)
{
	__int64 prevHead;
	__int64 newHead;
	Node* prevHeadNode;
	Node* newHeadNode;
	T data;

	while (true)
	{
		prevHeadNode = reinterpret_cast<Node*>(GET_PTR(prevHead));
		newHeadNode = reinterpret_cast<Node*>(GET_PTR(prevHeadNode->_next));
		data = newHeadNode->_value;

		if(Interlocked)
	}

	InterlockedDecrement64(&_size);
}
