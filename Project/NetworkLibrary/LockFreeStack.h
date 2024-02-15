#pragma once

#include "ObjectPool.h"

template<typename T>
class LockFreeStack
{
public:
	struct Node
	{
		T _data;
		__int64 _next;
	};

public:
	LockFreeStack();
	~LockFreeStack();

public:
	void Push(T data);
	T Pop(void);
	__int64 Size(void) const;

private:
	__int64 _top;
	__int64 _key;
	__int64 _size;

public:
	static ObjectPool<Node>* _stackNodePool;
};


template<typename T>
LockFreeStack<T>::LockFreeStack()
{
	//_stackNodePool = new ObjectPool<Node>(20000);
}

template<typename T>
LockFreeStack<T>::~LockFreeStack()
{
	//delete _stackNodePool;
}

template<typename T>
void LockFreeStack<T>::Push(T data)
{
	__int64 tempTop;
	__int64 newTop;
	__int64 key;
	Node* newNode;

	key = InterlockedIncrement64(&_key);

	newNode = _stackNodePool->Alloc();
	newNode->_data = data;

	newTop = GET_PTR((__int64)newNode);
	newTop = ((key << POOL_KEY_BITMASK_64BIT) | newTop);

	while (true)
	{
		tempTop = _top;
		newNode->_next = tempTop;

		if (InterlockedCompareExchange64(&_top, newTop, tempTop) == tempTop)
		{
			break;
		}
	}
}

template<typename T>
T LockFreeStack<T>::Pop(void)
{
	__int64 newTop;
	__int64 tempTop;
	Node* tempTopNode;


	while (true)
	{
		tempTop = _top;
		tempTopNode = (Node*)GET_PTR(tempTop);
		newTop = tempTopNode->_next;

		if (InterlockedCompareExchange64(&_top, newTop, tempTop) == tempTop)
		{
			T data = tempTopNode->_data;
			_stackNodePool->Free(tempTopNode);
			InterlockedDecrement64(&_size);

			return data;
		}
	}
}

template<typename T>
__int64 LockFreeStack<T>::Size(void) const
{
	return _size;
}

template<typename T>
ObjectPool<typename LockFreeStack<T>::Node>* LockFreeStack<T>::_stackNodePool = new ObjectPool<LockFreeStack<T>::Node>(20000);