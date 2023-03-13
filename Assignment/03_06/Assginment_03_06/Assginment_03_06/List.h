#pragma once

namespace MyDataStructure
{
	template<typename T>
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

			iterator operator ++(int)
			{
				iterator temp = *this;
				mNode = mNode->mNext;
				return temp;
			}

			iterator& operator++()
			{
				mNode->mNext;
				return *this;
			}

			iterator operator --(int)
			{
				iterator temp = *this;
				mNode = mNode->mPrev;
				return temp;
			}

			iterator& operator--()
			{
				mNode->mPrev;
				return *this;
			}

			T& operator *()
			{
				return mNode->mData;
			}

			bool operator ==(const iterator& other)
			{
				if (mNode->mData == other->mNode)
				{
					return true;
				}

				return false;
			}

			bool operator !=(const iterator& other)
			{
				if (mNode->mData != other->mNode)
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

		void push_front(T data);
		void push_back(T data);
		T pop_front(void);
		T pop_back(void);

		void insert(T data);
		iterator erase(iterator it);
		void clear(void);
		size_t size(void) const;
		bool empty(void) const;

		iterator begin(void) const;
		iterator end(void) const;

	private:
		size_t mSize;
		Node mHead;
		Node mTail;
	};


}
