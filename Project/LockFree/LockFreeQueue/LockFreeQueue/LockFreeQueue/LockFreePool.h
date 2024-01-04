#pragma once

#include <new>
#include <Windows.h>
#include <cstdio>

#define POOL_KEY_BITMASK_64BIT 47
#define GET_KEY(ptr) (((ptr) >> POOL_KEY_BITMASK_64BIT) & 0x1ffff)
#define GET_PTR(ptr) ((ptr) & (0x00007fffffffffff))

#define __LOCK_FREE_DEBUG__

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

	T* Alloc(void);
	void Free(T* obj);

public:
	__int64 _top;

private:
	__int64 _key;
	size_t _capacity;
};

template<typename T>
template<typename... Types>
LockFreePool<T>::LockFreePool(size_t capacity, Types... args)
	: _top(NULL)
	, _key(-1)
	, _capacity(capacity)
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

	__int64 key;

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
T* LockFreePool<T>::Alloc(void)
{
	__int64 tempTop;
	__int64 next;
	Node* curNode;
	T* ptr;

//#ifdef __LOCK_FREE_DEBUG__
//	__int64 idx;
//	idx = InterlockedIncrement64(&g_logIndex);
//	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
//	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
//	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_ALLOC;
//	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = 0;
//	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = 0;
//#endif

	while (true)
	{
		tempTop = _top;

		if (tempTop == NULL)
		{
			int* p = nullptr;
			*p = 0;
			return NULL;
		}

		curNode = (Node*)GET_PTR(tempTop);
		next = curNode->_next;

		if (InterlockedCompareExchange64(&_top, next, tempTop) == tempTop)
		{
//#ifdef __LOCK_FREE_DEBUG__
//			idx = InterlockedIncrement64(&g_logIndex);
//#endif
			//InterlockedIncrement64(&g_allocCnt);
			break;
		}
	}

//#ifdef __LOCK_FREE_DEBUG__
//	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
//	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
//	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_ALLOC;
//	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = (void*)tempTop;
//	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = (void*)next;
//#endif

	if (GET_PTR(tempTop) == GET_PTR(next))
	{
		__debugbreak();
	}

	ptr = &(curNode->_value);
	curNode->_next = 0;

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

//#ifdef __LOCK_FREE_DEBUG__
//	__int64 idx;
//	idx = InterlockedIncrement64(&g_logIndex);
//	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
//	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
//	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_FREE;
//	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = 0;
//	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = 0;
//#endif

	while (true)
	{
		tempTop = _top;
		newNode->_next = tempTop;

		if ((InterlockedCompareExchange64(&_top, newTop, tempTop) == tempTop))
		{
//#ifdef __LOCK_FREE_DEBUG__
//			idx = InterlockedIncrement64(&g_logIndex);
//#endif
			//InterlockedIncrement64(&g_freeCnt);
			break;
		}
	}

//#ifdef __LOCK_FREE_DEBUG__
//	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
//	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
//	g_logArray[idx % LOG_ARRAY_LEN]._jobType = JOB_FREE;
//	g_logArray[idx % LOG_ARRAY_LEN]._nodePtr = (void*)newTop;
//	g_logArray[idx % LOG_ARRAY_LEN]._nextPtr = (void*)tempTop;
//#endif

	if (GET_PTR(tempTop) == GET_PTR(newTop))
	{
		__debugbreak();
	}

	return;
}

