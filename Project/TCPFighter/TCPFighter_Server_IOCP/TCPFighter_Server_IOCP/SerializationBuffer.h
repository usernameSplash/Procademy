#pragma once

#include <Windows.h>
#include "Protocol.h"

// Packet Managed by Serialized Buffer.
class SPacket
{
public:
	enum eBufferDefault
	{
		BUFFER_MINIMUM_SIZE = 64,
		BUFFER_DEFAULT_SIZE = 512,
		BUFFER_MAX_SIZE = 4096,
	};

	SPacket();
	SPacket(size_t capacity);
	SPacket(const SPacket& other);
	virtual ~SPacket(void);

	void Clear(void);
	size_t Capacity(void);
	size_t Size(void);

	void Reserve(size_t size);

	char* GetBufferPtr(void);
	char* GetPayloadPtr(void);

	size_t MoveReadPos(size_t size);
	size_t MoveWritePos(size_t size);

	void SetHeaderData(void* header);
	void GetHeaderData(void* outHeader);

public:
	SPacket& operator=(SPacket& rhs);

	SPacket& operator<<(unsigned char data);
	SPacket& operator<<(char data);
	SPacket& operator<<(unsigned short data);
	SPacket& operator<<(short data);
	SPacket& operator<<(unsigned int data);
	SPacket& operator<<(int data);
	SPacket& operator<<(unsigned long data);
	SPacket& operator<<(long data);
	SPacket& operator<<(unsigned long long data);
	SPacket& operator<<(long long data);
	SPacket& operator<<(float data);
	SPacket& operator<<(double data);

	SPacket& operator>>(unsigned char& data);
	SPacket& operator>>(char& data);
	SPacket& operator>>(unsigned short& data);
	SPacket& operator>>(short& data);
	SPacket& operator>>(unsigned int& data);
	SPacket& operator>>(int& data);
	SPacket& operator>>(unsigned long& data);
	SPacket& operator>>(long& data);
	SPacket& operator>>(unsigned long long& data);
	SPacket& operator>>(long long& data);
	SPacket& operator>>(float& data);
	SPacket& operator>>(double& data);

protected:

	char* mBuffer;
	char* mPayloadPtr;
	char* mReadPos;
	char* mWritePos;

	size_t mCapacity;
	size_t mSize;
};