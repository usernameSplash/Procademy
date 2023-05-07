#include "RingBuffer.h"

#include <cstdio>
#include <cstring>

namespace MyDataStructure
{
	RingBuffer::RingBuffer(void)
		: mBuffer(new char[CAPACITY])
		, mFront(mBuffer)
		, mRear(mBuffer)
		, mCapacity(CAPACITY)
		, mSize(0)
	{
	}

	RingBuffer::RingBuffer(size_t capacity)
		: mBuffer(new char[capacity])
		, mFront(mBuffer)
		, mRear(mBuffer)
		, mCapacity(capacity)
		, mSize(0)
	{
	}

	void RingBuffer::Reserve(size_t capacity)
	{
		char* newBuffer;
		size_t frontPos;

		if (mCapacity >= capacity)
		{
			return;
		}

		newBuffer = new char[capacity];
		for (size_t iCnt = 0; iCnt < mSize; iCnt++)
		{
			newBuffer[iCnt] = mBuffer[iCnt];
		}

		frontPos = mFront - mBuffer;

		delete[] mBuffer;
		mBuffer = newBuffer;
		mFront = mBuffer + frontPos;
		mRear = mFront + mSize;

		mCapacity = capacity;
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
		// I made the mistake of getting a direct size for the old size instead of getting a direct size for the new(bigger) size
		if (mCapacity < (mSize + size))
		{
			// if new capacity is larger than 8 times its origin, enqueuing is refused
			if ((mSize + size) > (CAPACITY << 3))
			{
				(L"Enqueue Failed\n");
				return 0;
			}

			size_t tempCapacity = mCapacity;
			while (tempCapacity < (mSize + size))
			{
				tempCapacity <<= 1;
			}

			if (tempCapacity > (CAPACITY << 3))
			{
				tempCapacity = (CAPACITY << 3);
			}

			Reserve(tempCapacity);
		}

		const size_t DIRECT_ENQUEUE_SIZE = DirectEnqueueSize();
		size_t result = 0;
		//size_t srcDataIdx;

		if (DIRECT_ENQUEUE_SIZE < size)
		{
			memcpy(mRear, srcData, DIRECT_ENQUEUE_SIZE);
			//for (srcDataIdx = 0; srcDataIdx < DIRECT_ENQUEUE_SIZE; srcDataIdx++)
			//{
			//	*mRear = srcData[srcDataIdx];
			//	mRear++;
			//	result++;
			//}

			mRear = mBuffer;
			memcpy(mRear, srcData + DIRECT_ENQUEUE_SIZE, size - DIRECT_ENQUEUE_SIZE);
			mRear = mBuffer + (size - DIRECT_ENQUEUE_SIZE);

			//for (; srcDataIdx < size; srcDataIdx)
			//{
			//	*mRear = srcData[srcDataIdx];
			//	mRear++;
			//	result++;
			//}
		}
		else
		{
			memcpy(mRear, srcData, size);
			mRear = mRear + size;

			//for (srcDataIdx = 0; srcDataIdx < size; srcDataIdx++)
			//{
			//	*mRear = srcData[srcDataIdx];
			//	mRear++;
			//	result++;
			//}
		}

		result = size;
		mSize += result;

		return result;
	}

	size_t RingBuffer::Dequeue(size_t size)
	{
		const size_t DIRECT_DEQUEUE_SIZE = DirectDequeueSize();

		// if Dequeuing size is bigger than stored size, dequeuing size is decreased by stored size.
		if (mSize < size)
		{
			size = mSize;
		}

		if (DIRECT_DEQUEUE_SIZE < size)
		{
			mFront = mBuffer + (size - DIRECT_DEQUEUE_SIZE);
		}
		else
		{
			mFront = mFront + size;
		}

		mSize -= size;

		return size;
	}

	size_t RingBuffer::Peek(char* dstData, size_t size)
	{
		const size_t DIRECT_DEQUEUE_SIZE = DirectDequeueSize();
		size_t result = 0;
		//size_t dstDataIdx;

		char* peekPtr = mFront;

		if (mSize < size)
		{
			size = mSize;
		}

		if (DIRECT_DEQUEUE_SIZE < size)
		{
			memcpy(dstData, peekPtr, DIRECT_DEQUEUE_SIZE);
			//for (dstDataIdx = 0; dstDataIdx < DIRECT_DEQUEUE_SIZE; dstDataIdx++)
			//{
			//	dstData[dstDataIdx] = *peekPtr;
			//	peekPtr++;
			//}

			peekPtr = mBuffer;
			
			memcpy(dstData + DIRECT_DEQUEUE_SIZE, peekPtr, size - DIRECT_DEQUEUE_SIZE);
			//for (; dstDataIdx < size; dstDataIdx++)
			//{
			//	dstData[dstDataIdx] = *peekPtr;
			//	peekPtr++;
			//}
		}
		else
		{
			memcpy(dstData, peekPtr, size);
		}

		result = size;

		return result;
	}

	size_t RingBuffer::DirectEnqueueSize(void)
	{
		if (mRear < mFront)
		{
			return mFront - mRear;
		}

		return mBuffer + mCapacity - mRear;
	}

	size_t RingBuffer::DirectDequeueSize(void)
	{
		if (mRear < mFront)
		{
			return mBuffer + mCapacity - mFront;
		}

		return mRear - mFront;
	}

	void RingBuffer::ClearBuffer(void)
	{
		mFront = mBuffer;
		mRear = mBuffer;
		mSize = 0;
	}

	size_t RingBuffer::MoveFront(size_t size)
	{
		const size_t DIRECT_DEQUEUE_SIZE = DirectDequeueSize();

		if (DIRECT_DEQUEUE_SIZE < size)
		{
			mFront = mBuffer + size - DIRECT_DEQUEUE_SIZE;
		}
		else
		{
			mFront = mFront + size;
		}

		return size;
	}

	size_t RingBuffer::MoveRear(size_t size)
	{
		const size_t DIRECT_ENQUEUE_SIZE = DirectEnqueueSize();

		if (DIRECT_ENQUEUE_SIZE < size)
		{
			mRear = mBuffer + size - DIRECT_ENQUEUE_SIZE;
		}
		else
		{
			mRear = mRear + size;
		}

		return size;
	}

	char* RingBuffer::GetFrontBufferPtr(void)
	{
		return mFront;
	}

	char* RingBuffer::GetRearBufferPtr(void)
	{
		return mRear;
	}
}