#pragma once

#include <new>
#include <Windows.h>
#include <cstdio>

#define BLOCK_ARRAY_LEN_MAX 256

#define POOL_KEY_BITMASK_64BIT 47
#define GET_KEY(ptr) (((ptr) >> POOL_KEY_BITMASK_64BIT) & 0x1ffff)
#define GET_PTR(ptr) ((ptr) & (0x00007fffffffffff))

#define KEY_MASK 0xffff800000000000

template<typename T>
class ObjectPool
{
	struct Block
	{
		T* _objArr[BLOCK_ARRAY_LEN_MAX];
		__int64 _next;
	};

	class SubPool
	{
	public:
		SubPool(ObjectPool* parent)
			: _blockIdx(BLOCK_ARRAY_LEN_MAX)
			, _backBlock(nullptr)
			, _parent(parent)
		{
			_parent->GetBlock(&_frontBlock);
		}

	private:
		T* Alloc(void);
		void Free(T* data);
		int Size(void);

	private:
		ObjectPool* _parent;
		Block* _frontBlock;
		Block* _backBlock;
		DWORD _blockIdx;

		friend class ObjectPool;
	};

public:
	template<typename... Types>
	ObjectPool(size_t blockNum, Types... args);
	~ObjectPool();

public:
	T* Alloc(void);
	void Free(T* data);
	__int64 Size(void);
	__int64 NodeCount(void);

private:
	void GetBlock(Block** outBlock);
	void ReturnBlock(Block* block);
	void GetEmptyBlock(Block** outBlock);
	void ReturnEmptyBlock(Block* block);

private:

	DWORD _tlsIdx;
	LONG64 _blockNum;
	LONG64 _emptyBlockNum;
	size_t _capacity;

	__int64 _blockKey;
	__int64 _blockTop;
	__int64 _emptyBlockKey;
	__int64 _emptyBlockTop;

	long long _nodeCnt;

	friend class SubPool;
};

template<typename T>
T* ObjectPool<T>::SubPool::Alloc(void)
{
	if (_blockIdx == 0)
	{
		if (_backBlock != nullptr)
		{
			_frontBlock = _backBlock;
			_backBlock = nullptr;
		}
		else
		{
			_parent->ReturnEmptyBlock(_frontBlock);
			_parent->GetBlock(&_frontBlock);
		}
		_blockIdx = BLOCK_ARRAY_LEN_MAX;
	}

	return _frontBlock->_objArr[--_blockIdx];
}

template<typename T>
void ObjectPool<T>::SubPool::Free(T* data)
{
	if (_blockIdx == BLOCK_ARRAY_LEN_MAX)
	{
		if (_backBlock == nullptr)
		{
			_backBlock = _frontBlock;
			_parent->GetEmptyBlock(&_frontBlock);
		}
		else
		{
			_parent->ReturnBlock(_backBlock);
			_backBlock = _frontBlock;
		}
		_blockIdx = 0;
	}

	_frontBlock->_objArr[_blockIdx++] = data;
}

template<typename T>
int ObjectPool<T>::SubPool::Size(void)
{
	return (int)_blockIdx;
}

template<typename T>
template<typename... Types>
ObjectPool<T>::ObjectPool(size_t blockNum, Types... args)
	: _blockNum(blockNum)
	, _emptyBlockNum(0)
	, _capacity(blockNum* BLOCK_ARRAY_LEN_MAX)
	, _blockKey(-1)
	, _blockTop(NULL)
	, _emptyBlockKey(-1)
	, _emptyBlockTop(NULL)
	, _nodeCnt(0)
{
	_tlsIdx = TlsAlloc();

	for (int iCnt = 0; iCnt < blockNum; ++iCnt)
	{
		Block* block = new Block;
		block->_next = _blockTop;

		for (int jCnt = 0; jCnt < BLOCK_ARRAY_LEN_MAX; ++jCnt)
		{
			block->_objArr[jCnt] = new T;
		}

		__int64 key = InterlockedIncrement64(&_blockKey);
		__int64 blockTop = (__int64)block;
		blockTop = ((key << POOL_KEY_BITMASK_64BIT) | blockTop);
		_blockTop = blockTop;
	}
}

template<typename T>
ObjectPool<T>::~ObjectPool()
{
	__int64 tempTop = _blockTop;
	_blockTop = 0;

	while (true)
	{
		Block* tempTopBlock = (Block*)GET_PTR(tempTop);
		__int64 nextBlock = tempTopBlock->_next;

		if (tempTopBlock == nullptr)
		{
			break;
		}

		for (int iCnt = 0; iCnt < BLOCK_ARRAY_LEN_MAX; ++iCnt)
		{
			delete tempTopBlock->_objArr[iCnt];
		}

		delete tempTopBlock;
		tempTop = nextBlock;
	}
}

template<typename T>
T* ObjectPool<T>::Alloc(void)
{
	SubPool* pool = (SubPool*)TlsGetValue(_tlsIdx);

	if (pool == nullptr)
	{
		pool = new SubPool(this);

		BOOL ret = TlsSetValue(_tlsIdx, (LPVOID)pool);
		if (ret == FALSE)
		{
			DWORD errorCode = GetLastError();
			wprintf(L"TLS Set Value Error : %d\n", errorCode);
			__debugbreak();
		}
	}

	InterlockedIncrement64(&_nodeCnt);
	return pool->Alloc();
}

template<typename T>
void ObjectPool<T>::Free(T* data)
{
	SubPool* pool = (SubPool*)TlsGetValue(_tlsIdx);

	if (pool == nullptr)
	{
		pool = new SubPool(this);

		pool->_parent = this;

		BOOL ret = TlsSetValue(_tlsIdx, (LPVOID)pool);
		if (ret == FALSE)
		{
			DWORD errorCode = GetLastError();
			wprintf(L"TLS Set Value Error : %d\n", errorCode);
			__debugbreak();
		}
	}

	InterlockedDecrement64(&_nodeCnt);
	pool->Free(data);

	return;
}

template<typename T>
__int64 ObjectPool<T>::Size(void)
{
	return (size_t)_blockNum;
}

template<typename T>
__int64 ObjectPool<T>::NodeCount(void)
{
	return _nodeCnt;
}

template<typename T>
void ObjectPool<T>::GetBlock(Block** outBlock)
{
	__int64 tempTop;
	__int64 tempTopNext;
	Block* tempTopBlock;

	while (true)
	{
		tempTop = _blockTop;

		if (tempTop == NULL)
		{
			*outBlock = new Block;
			for (int iCnt = 0; iCnt < BLOCK_ARRAY_LEN_MAX; ++iCnt)
			{
				(*outBlock)->_objArr[iCnt] = new T;
			}
			break;
		}

		tempTopBlock = (Block*)GET_PTR(tempTop);
		tempTopNext = tempTopBlock->_next;

		if (InterlockedCompareExchange64(&_blockTop, tempTopNext, tempTop) == tempTop)
		{
			*outBlock = tempTopBlock;
			InterlockedDecrement64(&_blockNum);
			break;
		}
	}
}

template<typename T>
void ObjectPool<T>::ReturnBlock(Block* block)
{
	__int64 newTop;
	__int64 prevTop;

	__int64 key = InterlockedIncrement64(&_blockKey);
	newTop = (__int64)block;
	newTop = ((key << POOL_KEY_BITMASK_64BIT) | GET_PTR(newTop));

	while (true)
	{
		prevTop = _blockTop;
		block->_next = prevTop;

		if (InterlockedCompareExchange64(&_blockTop, newTop, prevTop) == prevTop)
		{
			InterlockedIncrement64(&_blockNum);
			break;
		}
	}

}

template<typename T>
void ObjectPool<T>::GetEmptyBlock(Block** outBlock)
{
	__int64 tempTop;
	__int64 tempTopNext;
	Block* tempTopBlock;

	while (true)
	{
		tempTop = _emptyBlockTop;

		if (tempTop == NULL)
		{
			*outBlock = new Block;
			break;
		}

		tempTopBlock = (Block*)GET_PTR(tempTop);
		tempTopNext = tempTopBlock->_next;

		if (InterlockedCompareExchange64(&_emptyBlockTop, tempTopNext, tempTop) == tempTop)
		{
			*outBlock = tempTopBlock;
			InterlockedDecrement64(&_emptyBlockNum);
			break;
		}
	}
}

template<typename T>
void ObjectPool<T>::ReturnEmptyBlock(Block* block)
{
	__int64 newTop;
	__int64 prevTop;

	__int64 key = InterlockedIncrement64(&_emptyBlockKey);
	newTop = (__int64)block;
	newTop = ((key << POOL_KEY_BITMASK_64BIT) | GET_PTR(newTop));

	while (true)
	{
		prevTop = _emptyBlockTop;
		block->_next = prevTop;

		if (InterlockedCompareExchange64(&_emptyBlockTop, newTop, prevTop) == prevTop)
		{
			InterlockedIncrement64(&_emptyBlockNum);
			break;
		}
	}
}