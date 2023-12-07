#pragma once
//#include <cstdlib>
//#include <new>

#define OBJECT_POOL_MIN_SIZE 64

template <typename T>
class ObjectPool
{
private:
	struct Node
	{
		T _data;
		Node* _next;
	};

public:
	ObjectPool(int capacity);
	~ObjectPool();

	T* Alloc(void);
	void Free(T* obj);

private:
	Node* _head;
};

template<typename T>
ObjectPool<T>::ObjectPool(int capacity)
{
	if (capacity <= OBJECT_POOL_MIN_SIZE)
	{
		capacity = OBJECT_POOL_MIN_SIZE;
	}

	_head = nullptr;

	for (int iCnt = 0; iCnt < capacity; ++iCnt)
	{
		Node* node = new Node;
		node->_next = _head;
		_head = node;
	}
}

template<typename T>
ObjectPool<T>::~ObjectPool()
{
	while (_head != nullptr)
	{
		Node* nextNode = _head->_next;
		delete _head;
		_head = nextNode;
	}
}

template<typename T>
T* ObjectPool<T>::Alloc(void)
{
	if (_head == nullptr)
	{
		T* newData = new T;
		return newData;
	}

	Node* node = _head;
	_head = _head->_next;

	return &(node->_data);
}

template<typename T>
void ObjectPool<T>::Free(T* obj)
{
	((Node*)obj)->_next = _head;
	_head = (Node*)obj;
}