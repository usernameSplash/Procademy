#include "RingBuffer.h"

#include <cstdio>
#include <cstring>

namespace MyDataStructure
{
	RingBuffer::RingBuffer(void)
		: mBuffer(new char[DEFAULT_CAPACITY])
		, mFront(0)
		, mRear(0)
		, mCapacity(DEFAULT_CAPACITY)
		, mSize(0)
	{
	}

	RingBuffer::RingBuffer(size_t capacity)
		: mBuffer(new char[capacity])
		, mFront(0)
		, mRear(0)
		, mCapacity(capacity)
		, mSize(0)
	{
	}

	bool RingBuffer::Reserve(size_t capacity)
	{
		char* newBuffer;

		if (capacity > MAX_CAPACITY)
		{
			return false;
		}

		if (capacity < mCapacity)
		{
			return false;
		}

		newBuffer = new char[capacity];

		if (mFront < mRear)
		{
			memcpy(newBuffer, (mBuffer + mFront), mSize);
		}
		else if (mFront >= mRear)
		{
			int size1 = (int)mCapacity - mFront;
			int size2 = (int)mRear;
			memcpy(newBuffer, (mBuffer + mFront), size1);
			memcpy((newBuffer + size1), mBuffer, size2);
		}

		delete[] mBuffer;
		mBuffer = newBuffer;

		mCapacity = capacity;

		mFront = 0;
		mRear = (int)mSize;

		return true;
	}

	size_t RingBuffer::Capacity(void)
	{
		return mCapacity;
	}

	size_t RingBuffer::Size(void)
	{
		return mSize;
	}

	size_t RingBuffer::Enqueue(char* srcData, size_t size)
	{
		if (mCapacity < (mSize + size))
		{
			if (!Reserve(mCapacity * 2))
			{
				wprintf(L"Enqueue Failed, Origin Size : %zu, Added Size : %zu, Capacity : %zu\n", mSize, size, mCapacity);
				return 0;
			}
		}

		const size_t DIRECT_ENQUEUE_SIZE = DirectEnqueueSize();
		size_t result = 0;
		//size_t srcDataIdx;

		if (DIRECT_ENQUEUE_SIZE < size)
		{
			memcpy((mBuffer + mRear), srcData, DIRECT_ENQUEUE_SIZE);
			memcpy(mBuffer, srcData + DIRECT_ENQUEUE_SIZE, size - DIRECT_ENQUEUE_SIZE);
		}
		else
		{
			memcpy(mBuffer + mRear, srcData, size);
		}

		result = size;
		mSize += result;
		mRear = (int)(mRear + size) % mCapacity;

		return result;
	}

	size_t RingBuffer::Dequeue(size_t size)
	{
		// if Dequeuing size is bigger than stored size, dequeuing size is decreased by stored size.
		if (mSize < size)
		{
			size = mSize;
		}

		mFront = (int)(mFront + size) % mCapacity;
		mSize -= size;

		return size;
	}

	size_t RingBuffer::Peek(char* dstData, size_t size)
	{
		const size_t DIRECT_DEQUEUE_SIZE = DirectDequeueSize();
		size_t result = 0;
		//size_t dstDataIdx;

		if (mSize < size)
		{
			size = mSize;
		}

		if (DIRECT_DEQUEUE_SIZE < size)
		{
			memcpy(dstData, mBuffer + mFront, DIRECT_DEQUEUE_SIZE);
			memcpy(dstData + DIRECT_DEQUEUE_SIZE, mBuffer, size - DIRECT_DEQUEUE_SIZE);
		}
		else
		{
			memcpy(dstData, mBuffer + mFront, size);
		}

		result = size;

		return result;
	}

	size_t RingBuffer::DirectEnqueueSize(void)
	{
		if (mRear > mFront)
		{
			return mCapacity - mRear;
		}
		else if (mRear < mFront)
		{
			return mFront - mRear;
		}
		else
		{
			if (mSize == 0)
			{
				return mCapacity - mRear;
			}
			else
			{
				return 0;
			}
		}
	}

	size_t RingBuffer::DirectDequeueSize(void)
	{
		if (mRear > mFront)
		{
			return mRear - mFront;
		}
		else if (mRear < mFront)
		{
			return mCapacity - mFront;
		}
		else
		{
			if (mSize == 0)
			{
				return 0;
			}
			else
			{
				return mCapacity - mFront;
			}
		}
	}

	void RingBuffer::ClearBuffer(void)
	{
		mFront = 0;
		mRear = 0;
		mSize = 0;
	}

	size_t RingBuffer::MoveFront(size_t size)
	{
		if (mSize < size)
		{
			size = mSize;
		}

		mFront = (int)(mFront + size) % mCapacity;

		mSize -= size;

		return size;
	}

	size_t RingBuffer::MoveRear(size_t size)
	{
		if(size > MAX_CAPACITY)

		if ((mCapacity - mSize) < size)
		{
			size = (mCapacity - mSize);
		}

		mRear = (int)(mRear + size) % mCapacity;

		mSize += size;

		return size;
	}

	char* RingBuffer::GetBufferPtr(void)
	{
		return mBuffer;
	}

	char* RingBuffer::GetFrontBufferPtr(void)
	{
		return mBuffer + mFront;
	}

	char* RingBuffer::GetRearBufferPtr(void)
	{
		return mBuffer + mRear;
	}
}