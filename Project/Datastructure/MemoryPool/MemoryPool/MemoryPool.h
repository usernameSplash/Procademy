#pragma once

#include <new>

namespace MyDataStructure
{
	template <typename T>
	class MemoryPool
	{
	private:
		struct Node
		{
			T _val;
			Node* _next;
		};

	public:
		MemoryPool(size_t capacity = 0, bool bDeallocInFree = false);
		~MemoryPool();

		T* Alloc(void);
		void Free(T* ptr);

		size_t Capacity(void);
		size_t Size(void);

	private:
		Node* _topNode;
		size_t _capacity;
		size_t _size;
		bool _bDeallocInFree;
	};

	template<typename T>
	inline MemoryPool<T>::MemoryPool(size_t capacity, bool bDeallocInFree)
		: _topNode(nullptr)
		, _capacity(capacity)
		, _size(capacity)
		, _bDeallocInFree(bDeallocInFree)
	{
		for (size_t iCnt = 0; iCnt < capacity; iCnt++)
		{
			Node* newNode;
			newNode = new Node { T{}, _topNode };

			_topNode = newNode;
		}
	}


	template<typename T>
	inline MemoryPool<T>::~MemoryPool()
	{
		while (_topNode != nullptr)
		{
			Node* next = _topNode->_next;
			delete _topNode;

			_topNode = next;
		}
	}

	template<typename T>
	inline T* MemoryPool<T>::Alloc(void)
	{
		Node* returnNode;

		if (_size != 0)
		{
			returnNode = _topNode;
			_topNode = _topNode->_next;

			if (_bDeallocInFree)
			{
				returnNode = reinterpret_cast<Node*> (new (_topNode) T {});
			}

			_size--;
		}
		else
		{
			returnNode = new Node { T{}, nullptr };
			_capacity++;
		}

		return reinterpret_cast<T*>(returnNode);
	}

	template<typename T>
	inline void MemoryPool<T>::Free(T* ptr)
	{
		Node* newNode = reinterpret_cast<Node*>(ptr);
		newNode->_next = _topNode;

		_topNode = newNode;
		
		if (_bDeallocInFree)
		{
			ptr->~T();
		}

		_size++;

		return;
	}

	template<typename T>
	inline size_t MemoryPool<T>::Capacity(void)
	{
		return _capacity;
	}

	template<typename T>
	inline size_t MemoryPool<T>::Size(void)
	{
		return _size;
	}
}