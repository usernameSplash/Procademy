#include "SerializationBuffer.h"

#include <cstdio>
#include <cstring>

namespace MyDataStruct
{
	SPacket::SPacket(size_t headerSize)
		: mBuffer(new char[BUFFER_DEFAULT_SIZE])
		, mHeaderSize(headerSize)
		, mPayloadPtr(mBuffer + headerSize)
		, mReadPos(mPayloadPtr)
		, mWritePos(mPayloadPtr)
		, mCapacity(BUFFER_DEFAULT_SIZE)
		, mPayloadCapacity(mCapacity - headerSize)
		, mSize(headerSize)
		, mPayloadSize(0)
	{
	}

	SPacket::SPacket(size_t headerSize, size_t capacity)
		: mHeaderSize(headerSize)
		, mSize(headerSize)
		, mPayloadSize(0)
	{
		if (capacity < BUFFER_MINIMUM_SIZE)
		{
			capacity = BUFFER_MINIMUM_SIZE;
		}
		else if (capacity > BUFFER_MAX_SIZE)
		{
			capacity = BUFFER_MAX_SIZE;
		}

		mBuffer = new char[capacity];
		mPayloadPtr = mBuffer + headerSize;
		mReadPos = mPayloadPtr;
		mWritePos = mPayloadPtr;
		mCapacity = capacity;
		mPayloadCapacity = capacity - headerSize;
	}

	SPacket::SPacket(const SPacket& other)
		: mBuffer(new char[other.mCapacity])
		, mPayloadPtr(mBuffer + other.mHeaderSize)
		, mReadPos(mPayloadPtr)
		, mWritePos(mPayloadPtr + other.mSize)
		, mHeaderSize(other.mHeaderSize)
		, mCapacity(other.mCapacity)
		, mPayloadCapacity(other.mPayloadCapacity)
		, mSize(other.mSize)
		, mPayloadSize(other.mPayloadSize)
	{
		memcpy(mBuffer, other.mBuffer, mSize);
	}

	SPacket::~SPacket(void)
	{
		delete mBuffer;
		mBuffer = nullptr;
	}

	void SPacket::Clear(void)
	{
		mSize = mHeaderSize;
		mPayloadSize = 0;
		return;
	}

	size_t SPacket::Capacity(void)
	{
		return mPayloadCapacity;
	}

	size_t SPacket::Size(void)
	{
		return mPayloadSize;
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

		memcpy(newBuffer, mBuffer, mSize);

		mCapacity = capacity;
		mPayloadCapacity = capacity - mHeaderSize;
		
		delete mBuffer;

		mBuffer = newBuffer;
		mPayloadPtr = mBuffer + mHeaderSize;
		mReadPos = mPayloadPtr;
		mWritePos = mPayloadPtr + mSize;

		return;
	}

	char* SPacket::GetBufferPtr(void)
	{
		return mBuffer;
	}

	char* SPacket::GetPayloadPtr(void)
	{
		return mPayloadPtr;
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

	void SPacket::SetHeaderData(void* header)
	{
		memcpy(mBuffer, header, mHeaderSize);
	}

	void SPacket::GetHeaderData(void* outHeader)
	{
		memcpy(outHeader, mBuffer, mHeaderSize);
	}

	SPacket& SPacket::operator=(SPacket& rhs)
	{
		if (this != &rhs)
		{
			delete mBuffer;

			mBuffer = new char[rhs.mCapacity];
			mPayloadPtr = mBuffer + mHeaderSize;
			mReadPos = mPayloadPtr;
			mWritePos = mPayloadPtr;
			mCapacity = rhs.mCapacity;
			mPayloadCapacity = rhs.mPayloadCapacity;
			mSize = rhs.mSize;
			mPayloadSize = rhs.mPayloadSize;
		}
	
		return *this;
	}

	SPacket& SPacket::operator<<(unsigned char data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(char data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned short data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(short data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned int data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(int data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned long data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(long data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(unsigned long long data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(long long data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(float data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
		mWritePos += sizeof(data);

		return *this;
	}

	SPacket& SPacket::operator<<(double data)
	{
		if (mPayloadPtr != mReadPos)
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

			if ((mCapacity - mHeaderSize) * 2 > BUFFER_MAX_SIZE)
			{
				Reserve(BUFFER_MAX_SIZE);
			}
			else
			{
				Reserve((mCapacity - mHeaderSize) * 2);
			}
		}

		memcpy(mWritePos, &data, sizeof(data));
		mSize += sizeof(data);
		mPayloadSize += sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
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
		mPayloadSize -= sizeof(data);
		mReadPos += sizeof(data);

		return *this;
	}
 }