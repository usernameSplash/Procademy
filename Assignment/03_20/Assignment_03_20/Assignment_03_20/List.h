#pragma once

namespace MyDataStructure
{
	template <typename T>
	class List
	{
	private:
		struct Node
		{
			T mData;
			Node* mPrev;
			Node* mNext;
		};

	public:
		class iterator
		{
		private:
			Node* mNode;
		public:
			iterator(Node* node = nullptr)
			{
				mNode = node;
			}

			iterator operator++(int)
			{
				iterator temp = *this;
				mNode = mNode->mNext;
				return temp;
			}

			iterator& operator++()
			{
				mNode = mNode->mNext;
				return *this;
			}

			iterator operator--(int)
			{
				iterator temp = *this;
				mNode = mNode->mPrev;
				return temp;
			}

			iterator& operator--()
			{
				mNode = mNode->mPrev;
				return *this;
			}

			T& operator*()
			{
				return mNode->mData;
			}

			T& operator->()
			{
				return mNode->mData;
			}

			bool operator==(const iterator& other)
			{
				if (mNode->mData == other.mNode)
				{
					return true;
				}

				return false;
			}

			bool operator!=(const iterator& other)
			{
				if (mNode->mData != other.mNode->mData)
				{
					return true;
				}

				return false;
			}
		};

	public:
		List();
		List(const List<T>& other);
		List<T>& operator=(const List<T>& rhs);
		~List();

		iterator begin(void);
		iterator end(void);

		void push_front(T data);
		void push_back(T data);
		T pop_front(void);
		T pop_back(void);

		void clear(void);
		size_t size(void) const;
		bool empty(void) const;

		iterator erase(iterator it);
		void remove(T data);

	private:
		size_t mSize;
		Node mHead;
		Node mTail;
	};

	template <typename T>
	List<T>::List()
		: mSize(0)
	{
		mHead.mPrev = nullptr;
		mHead.mNext = &mTail;

		mTail.mPrev = &mHead;
		mTail.mNext = nullptr;
	}

	template <typename T>
	List<T>::List(const List<T>& other)
		: mSize(other.mSize)
	{
		mHead.mPrev = nullptr;
		mHead.mNext = &mTail;

		mTail.mPrev = &mHead;
		mTail.mNext = nullptr;

		for (iterator it = other.begin(); it != other.end(); ++it)
		{
			Node* node = new Node();
			node->mData = *it;
			node->mPrev = mTail.mPrev;
			node->mNext = mTail;

			mTail.mPrev->mNext = node;
			mTail.mPrev = node;
		}
	}

	template <typename T>
	List<T>& List<T>::operator=(const List<T>& rhs)
	{
		if (this != &rhs)
		{
			mSize = rhs.mSize;

			for (iterator it = begin(); it != end(); ++it)
			{
				delete (*it);
			}
			mHead.mNext = &mTail;
			mTail.mPrev = &mHead;

			for (iterator it = rhs.begin(); it != rhs.end(); ++it)
			{
				Node* node = new Node();
				node->mData = *it;
				node->mPrev = mTail.mPrev;
				node->mNext = mTail;

				mTail.mPrev->mNext = node;
				mTail.mPrev = node;
			}
		}

		return *this;
	}

	template <typename T>
	List<T>::~List()
	{
		Node* cur = mHead.mNext;

		for (int iCnt = 0; iCnt < mSize; iCnt++)
		{
			Node* next = cur->mNext;
			delete cur;
			cur = next;
		}
	}

	template <typename T>
	typename List<T>::iterator List<T>::begin(void)
	{
		List<T>::iterator it(mHead.mNext);
		return it;
	}

	template <typename T>
	typename List<T>::iterator List<T>::end(void)
	{
		List<T>::iterator it(&mTail);
		return it;
	}

	template <typename T>
	void List<T>::push_front(T data)
	{
		Node* node = new Node();
		node->mData = data;

		node->mNext = mHead.mNext;
		mHead.mNext->mPrev = node;

		node->mPrev = &mHead;
		mHead.mNext = node;

		mSize++;
	}

	template <typename T>
	void List<T>::push_back(T data)
	{
		Node* node = new Node();
		node->mData = data;

		node->mPrev = mTail.mPrev;
		mTail.mPrev->mNext = node;

		node->mNext = &mTail;
		mTail.mPrev = node;

		mSize++;
	}

	template <typename T>
	T List<T>::pop_front(void)
	{
		if (mSize == 0)
		{
			return T();
		}

		Node* targetNode = mHead.mNext;
		T returnValue = targetNode->mData;

		mHead.mNext = targetNode->mNext;
		targetNode->mNext->mPrev = &mHead;

		delete targetNode;

		mSize--;

		return returnValue;
	}

	template <typename T>
	T List<T>::pop_back(void)
	{
		if (mSize == 0)
		{
			return;
		}

		Node* targetNode = mTail.mPrev;
		T returnValue = targetNode->mData;

		mTail.mPrev = targetNode->mPrev;
		targetNode->mPrev->mNext = &mTail;

		delete targetNode;

		mSize--;

		return returnValue;
	}

	template <typename T>
	void List<T>::clear(void)
	{
		Node* cur = mHead.mNext;

		for (int iCnt = 0; iCnt < mSize; iCnt++)
		{
			Node* next = cur->mNext;
			delete cur;
			cur = next;
		}

		mSize = 0;
		mHead.mNext = &mTail;
		mTail.mPrev = &mHead;
	}

	template <typename T>
	size_t List<T>::size(void) const
	{
		return mSize;
	}

	template <typename T>
	bool List<T>::empty(void) const
	{
		return mHead->mNext == &mTail;
	}

	template <typename T>
	typename List<T>::iterator List<T>::erase(iterator it)
	{
		Node* node = *it;
		node->mPrev->mNext = node->mNext;
		node->mNext->mPrev = node->mPrev;

		it.mNode = node->mNext;
		delete node;

		return it;
	}

	template <typename T>
	void List<T>::remove(T data)
	{
		List<T>::iterator it;
		for (it = begin(); it != end(); ++it)
		{
			if (*it == data)
			{
				erase(it);
			}
		}
	}
}
