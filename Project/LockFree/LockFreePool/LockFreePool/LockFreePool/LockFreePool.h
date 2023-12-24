#pragma once

#include <new>
#include <Windows.h>
#include <cstdio>

#define POOL_KEY_BITMASK_64BIT 47
#define GET_KEY(ptr) (((ptr) >> POOL_KEY_BITMASK_64BIT) & 0x1ffff)
#define GET_PTR(ptr) ((ptr) & ~(0xffffffffffffffff << POOL_KEY_BITMASK_64BIT))

template<typename T>
class LockFreePool
{
private:
	struct Node
	{
		T _value;
		__int64 _next;
	};

public:
	template<typename... Types>
	LockFreePool(size_t capacity, Types... args);
	~LockFreePool();

	template<typename... Types>
	T* Alloc(Types... args);
	void Free(T* obj);

private:
	__int64 _top;
	__int64 _key;
	size_t _capacity;
};

template<typename T>
template<typename... Types>
LockFreePool<T>::LockFreePool(size_t capacity, Types... args)
	: _capacity(capacity)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);

	LPVOID* tempAddr;
	tempAddr = reinterpret_cast<LPVOID*>(0xffffffffffffffff >> (64 - POOL_KEY_BITMASK_64BIT)); // 0x00007fffffffffff
	if (tempAddr < info.lpMaximumApplicationAddress)
	{
		wprintf(L"POOL KEY BITMASK ERROR\n");
		int* p = nullptr;
		*p = 0;
		return;
	}

	if (capacity <= 0)
	{
		return;
	}

	_top = NULL;
	_key = 0;

	__int64 key = 0;

	for (size_t iCnt = 0; iCnt < capacity; ++iCnt)
	{
		key = InterlockedIncrement64(&_key);
		key <<= POOL_KEY_BITMASK_64BIT;

		Node* newNode = (Node*)malloc(sizeof(Node));

		new (&(newNode->_value)) T(args...);
		newNode->_next = _top;

		_top = (__int64)((key) | (__int64)newNode);
	}
}

template<typename T>
LockFreePool<T>::~LockFreePool()
{
	Node* curNode = (Node*)(GET_PTR(_top));
	while (curNode != NULL)
	{
		Node* nextNode = (Node*)GET_PTR(curNode->_next);

		(curNode->_value).~T();
		free(curNode);

		curNode = nextNode;
	}
}

template<typename T>
template<typename... Types>
T* LockFreePool<T>::Alloc(Types... args)
{
	__int64 tempTop;
	__int64 next;
	Node* curNode;
	T* ptr;

	do
	{
		tempTop = _top;
		curNode = (Node*)GET_PTR(tempTop);
		next = curNode->_next;

		if (tempTop == NULL)
		{
			return NULL;
		}

	} while (InterlockedCompareExchange64(&_top, next, tempTop) != tempTop);

	ptr = &(curNode->_value);

	return ptr;
}

template<typename T>
void LockFreePool<T>::Free(T* obj)
{
	__int64 key;
	__int64 newTop;
	__int64 tempTop;
	Node* newNode;

	key = InterlockedIncrement64(&_key);
	
	newTop = (__int64)obj;
	newTop = GET_PTR(newTop);
	newNode = reinterpret_cast<Node*>(newTop);
	newTop = ((key << POOL_KEY_BITMASK_64BIT) | newTop);

	do
	{
		tempTop = _top;
		newNode->_next = tempTop;
	} while (InterlockedCompareExchange64(&_top, newTop, tempTop) != tempTop);

	return;
}

