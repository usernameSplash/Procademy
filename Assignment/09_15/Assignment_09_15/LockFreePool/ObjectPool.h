#pragma once
#include <Windows.h>

#define OBJECT_POOL_MIN_SIZE 64

template <typename T>
class LockFreePool
{
private:
	struct Node
	{
		T _data;
		Node* _next;
	};

public:
	LockFreePool(int capacity);
	~LockFreePool();

	T* Alloc(void);
	void Free(T* obj);

private:
	Node* _head;
};

template<typename T>
LockFreePool<T>::LockFreePool(int capacity)
{
	if (capacity <= OBJECT_POOL_MIN_SIZE)
	{
		capacity = OBJECT_POOL_MIN_SIZE;
	}

	_head = new Node;
	_head->_next = nullptr;

	for (int iCnt = 0; iCnt < capacity; ++iCnt)
	{
		Node* node = new Node;
		node->_next = _head;
		_head = node;
	}
}

template<typename T>
LockFreePool<T>::~LockFreePool()
{
	while (_head != nullptr)
	{
		Node* nextNode = _head->_next;
		delete _head;
		_head = nextNode;
	}
}

template<typename T>
T* LockFreePool<T>::Alloc(void)
{
	Node* head;
	Node* next;

	do
	{
		head = _head;

		if (head == nullptr)
		{
			T* newData = new T;
			return newData;
		}

		next = head->_next;
	} while (InterlockedCompareExchange64((LONG64*)&_head, (LONG64)next, (LONG64)head);

	return &(node->_data);
}

template<typename T>
void LockFreePool<T>::Free(T* obj)
{
	Node* node = static_cast<Node*>(obj);
	Node* head;

	do
	{
		head = _head;
		node->_next = head;
	} while (InterlockedCompareExchange64((LONG64*)&_head, (LONG64)node, (LONG64)head);
}