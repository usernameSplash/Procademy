#include "SPacket.h"

#include <cstdio>
#include <cstring>

ObjectPool<SPacket> SPacket::_packetPool = ObjectPool<SPacket>(20000);

SPacket::SPacket()
	: mBuffer(new char[BUFFER_DEFAULT_SIZE])
	, mPayloadPtr(mBuffer + sizeof(PacketHeader))
	, mReadPos(mPayloadPtr)
	, mWritePos(mPayloadPtr)
	, mCapacity(BUFFER_DEFAULT_SIZE)
	, mHeaderLen(sizeof(PacketHeader))
	, mHeaderSize(0)
	, mSize(sizeof(PacketHeader))
	, mUseCnt(0)
	, mIsEncoded(0)
{

}

SPacket::SPacket(size_t headerLen)
	: mBuffer(new char[BUFFER_DEFAULT_SIZE])
	, mPayloadPtr(mBuffer + headerLen)
	, mReadPos(mPayloadPtr)
	, mWritePos(mPayloadPtr)
	, mCapacity(BUFFER_DEFAULT_SIZE)
	, mHeaderLen(headerLen)
	, mHeaderSize(0)
	, mSize(headerLen)
	, mUseCnt(0)
	, mIsEncoded(0)
{
}

SPacket::SPacket(size_t headerLen, size_t capacity)
	: mHeaderLen(headerLen)
	, mHeaderSize(0)
	, mSize(headerLen)
	, mUseCnt(0)
	, mIsEncoded(0)
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
	mPayloadPtr = mBuffer + headerLen;
	mReadPos = mPayloadPtr;
	mWritePos = mPayloadPtr;
	mCapacity = capacity;
}

SPacket::SPacket(const SPacket& other)
	: mBuffer(new char[other.mCapacity])
	, mPayloadPtr(mBuffer + other.mHeaderLen)
	, mReadPos(mPayloadPtr)
	, mWritePos(mPayloadPtr + other.mSize)
	, mCapacity(other.mCapacity)
	, mSize(other.mSize)
	, mUseCnt(other.mUseCnt)
	, mIsEncoded(other.mIsEncoded)
{
	memcpy(mBuffer, other.mBuffer, mSize);
}

SPacket::~SPacket(void)
{
	delete mBuffer;
	mBuffer = nullptr;
}

bool SPacket::Encode(PacketHeader& header)
{
	if (mIsEncoded == 1)
	{
		return false;
	}

	if (InterlockedExchange(&mIsEncoded, 1) == 1)
	{
		return false;
	}

	unsigned char checkSum = 0;
	unsigned char* payloadPtr = (unsigned char*)GetPayloadPtr();
	
	for (int iCnt = 0; iCnt < header._len; ++iCnt)
	{
		checkSum += *(payloadPtr + iCnt);
	}
	checkSum = (checkSum % 256);

	unsigned char d = checkSum;
	unsigned char p = d ^ (header._randKey + 1);
	unsigned char e = p ^ (ENCODE_KEY + 1);
	header._checkSum = e;

	for (int iCnt = 0; iCnt < header._len; ++iCnt)
	{
		d = *(payloadPtr + iCnt);
		p = d ^ (p + header._randKey + (iCnt + 2));
		e = p ^ (e + ENCODE_KEY + (iCnt + 2));
		*(payloadPtr + iCnt) = e;
	}

	return true;
}

bool SPacket::Decode(PacketHeader& header)
{
	unsigned char checkSum = 0;
	unsigned char* payloadPtr = (unsigned char*)GetPayloadPtr();

	unsigned char e = header._checkSum;
	unsigned char p = e ^ (ENCODE_KEY + 1);
	unsigned char d = p ^ (header._randKey + 1);

	header._checkSum = e;

	for (int iCnt = 0; iCnt < header._len; ++iCnt)
	{
		e = *(payloadPtr + iCnt);
		p = e ^ (ENCODE_KEY + (iCnt + 2));
		d = p ^ (header._randKey + (iCnt + 2));

		*(payloadPtr + iCnt) = d;
		checkSum += d;
	}

	checkSum = (checkSum % 256);

	if (checkSum != header._checkSum)
	{
		return false;
	}

	return true;
}

void SPacket::Clear(void)
{
	mSize = mHeaderLen;
	mReadPos = mPayloadPtr;
	mWritePos = mPayloadPtr;
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

size_t SPacket::GetHeaderSize(void)
{
	return mHeaderSize;
}

size_t SPacket::GetPayloadSize(void)
{
	return mSize - mHeaderLen;
}

bool SPacket::Reserve(size_t capacity)
{
	char* newBuffer;

	if (mCapacity > capacity)
	{
		return false;
	}

	if (capacity > BUFFER_MAX_SIZE)
	{
		return false;
	}

	newBuffer = new char[capacity];

	memcpy(newBuffer, mBuffer, mSize);

	mCapacity = capacity;

	delete mBuffer;

	mBuffer = newBuffer;
	mPayloadPtr = mBuffer + mHeaderLen;
	mReadPos = mPayloadPtr;
	mWritePos = mPayloadPtr + mSize;

	return true;
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
	memcpy(mBuffer, header, sizeof(mHeaderLen));
}

void SPacket::GetHeaderData(void* outHeader)
{
	memcpy(outHeader, mBuffer, mHeaderSize);
}

size_t SPacket::GetHeaderData(void* outHeader, size_t size)
{
	if (size > mHeaderSize)
	{
		size = mHeaderSize;
	}

	memcpy(outHeader, mBuffer, size);

	return size;
}

void SPacket::SetPayloadData(void* data, size_t size)
{
	memcpy(mPayloadPtr, data, size);
}

void SPacket::GetPayloadData(void* outData)
{
	memcpy(outData, mPayloadPtr, (mSize - mHeaderLen));
}

size_t SPacket::GetPayloadData(void* outData, size_t size)
{
	if (size > mSize - mHeaderLen)
	{
		size = mSize - mHeaderLen;
	}

	memcpy(outData, mPayloadPtr, size);

	return size;
}

void SPacket::AddUseCnt(long cnt)
{
	InterlockedAdd(&mUseCnt, cnt);
}


SPacket& SPacket::operator=(SPacket& rhs)
{
	if (this != &rhs)
	{
		delete mBuffer;

		mBuffer = new char[rhs.mCapacity];
		mPayloadPtr = mBuffer + mHeaderLen;
		mReadPos = mPayloadPtr;
		mWritePos = mPayloadPtr;
		mCapacity = rhs.mCapacity;
		mSize = rhs.mSize;
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
		}
	}

	memcpy(mWritePos, &data, sizeof(data));
	mSize += sizeof(data);
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
		if (!Reserve(mCapacity * 2))
		{
#ifdef DEBUG
			wprintf(L"[SPacket] : Input Error, Data : %u\n", data);
#endif
			return *this;
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

SPacket* SPacket::Alloc(void)
{
	SPacket* packet = _packetPool.Alloc();
	return packet;
}

bool SPacket::Free(SPacket* packet)
{
	if (InterlockedDecrement(&packet->mUseCnt) == 0)
	{
		_packetPool.Free(packet);
		return true;
	}
	else
	{
		return false;
	}
}

int SPacket::PoolBlockSize(void)
{
	return (int)_packetPool.Size();
}

int SPacket::PoolNodeSize(void)
{
	return _packetPool.NodeCount();
}