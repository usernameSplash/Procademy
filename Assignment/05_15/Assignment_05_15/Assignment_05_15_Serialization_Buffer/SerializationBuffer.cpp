#include "SerializationBuffer.h"

#include <cstdio>
#include <cstring>

namespace MyDataStruct
{
	SPacket::SPacket(void)
		: mBuffer(new char[BUFFER_DEFAULT_SIZE])
		, mReadPos(mBuffer)
		, mWritePos(mBuffer)
		, mCapacity(BUFFER_DEFAULT_SIZE)
		, mSize(0)
	{

	}

	SPacket::SPacket(size_t capacity)
		: mSize(0)
	{
		if (capacity < BUFFER_DEFAULT_SIZE)
		{
			capacity = BUFFER_DEFAULT_SIZE;
		}
		else if (capacity > BUFFER_MAX_SIZE)
		{
			capacity = BUFFER_MAX_SIZE;
		}

		mBuffer = new char[capacity];
		mReadPos = mBuffer;
		mWritePos = mBuffer;
		mCapacity = capacity;
	}

	SPacket::SPacket(const SPacket& other)
		: mBuffer(new char[other.mCapacity])
		, mReadPos(mBuffer)
		, mWritePos(mBuffer + other.mSize)
		, mCapacity(other.mCapacity)
		, mSize(other.mSize)
	{
		char* srcPtr = other.mBuffer;
		char* dstPtr = mBuffer;

		size_t iCnt = 0;

		while (iCnt < mSize)
		{
			dstPtr = srcPtr;
			
			iCnt++;
			srcPtr++;
			dstPtr++;
		}
	}

	SPacket::~SPacket(void)
	{
		delete mBuffer;
		mBuffer = nullptr;
	}

	void SPacket::Clear(void)
	{
		mSize = 0;
		return;
	}

	size_t SPacket::Capacity(void)
	{
		return mCapacity;
	}

	size_t SPacket::Size(void)
	{
		return mSize;
	}

	void SPacket::Reserve(size_t capacity)
	{
		if (mCapacity > capacity)
		{
			return;
		}

		if (capacity > BUFFER_MAX_SIZE)
		{
			capacity = BUFFER_MAX_SIZE;
		}

		char* newBuffer = new char[capacity];

		char* srcPtr = mBuffer;
		char* dstPtr = newBuffer;
		size_t iCnt = 0;

		while (iCnt < mSize)
		{
			dstPtr = srcPtr;

			iCnt++;
			srcPtr++;
			dstPtr++;
		}

		mCapacity = capacity;
		
		delete mBuffer;

		mBuffer = newBuffer;
		mReadPos = mBuffer;
		mWritePos = mBuffer + mSize;

		return;
	}

	char* SPacket::GetBufferPtr(void)
	{
		return mBuffer;
	}

	size_t SPacket::MoveReadPos(size_t size)
	{
		if (size > mSize)
		{
			size = mSize;
		}

		mSize -= size;
		mReadPos += size;

		return size;
	}

	size_t SPacket::MoveWritePos(size_t size)
	{
		size_t freeSize = mCapacity - mSize;

		if (size > freeSize)
		{
			size = freeSize;
		}

		mSize += size;
		mWritePos += size;

		return size;
	}

	SPacket& SPacket::operator=(SPacket& rhs)
	{
		if (this != &rhs)
		{
			delete mBuffer;

			mBuffer = new char[rhs.mCapacity];
			mReadPos = mBuffer;
			mWritePos = mBuffer;
			mCapacity = rhs.mCapacity;
			mSize = rhs.mSize;
		}
	
		return *this;
	}

	SPacket& SPacket::operator<<(unsigned char data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(char data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %d\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned short data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(short data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %d\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned int data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(int data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %d\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned long data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(long data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %d\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned long long data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %llu\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(long long data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %lld\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(float data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %f\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(double data)
	{
		if (mBuffer != mReadPos)
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Do Not Input Additional Data After Any Output Behavior\n");
#endif
			return *this;
		}

		if ((mSize + sizeof(data) > mCapacity))
		{
			if (mSize + sizeof(data) > BUFFER_MAX_SIZE)
			{
#ifdef DEBUG
				wprintf(L"[SPacket] : Input Error, Data : %lf\n", data);
#endif
				return *this;
			}

			if (mCapacity * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve(mCapacity * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(unsigned char& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(unsigned char*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(char& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *mReadPos;
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(unsigned short& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(unsigned short*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(short& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(short*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(unsigned int& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(unsigned int*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}


	SPacket& SPacket::operator>>(int& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(int*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(unsigned long& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(unsigned long*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(long& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(long*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}


	SPacket& SPacket::operator>>(unsigned long long& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(unsigned long long*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}


	SPacket& SPacket::operator>>(long long& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(long long*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(float& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(float*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator>>(double& data)
	{
		if (sizeof(data) > mSize)
		{
			return *this;
		}

		data = *(double*)(mReadPos);
		mSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}
 }