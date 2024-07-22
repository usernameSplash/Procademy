#pragma once

#include <WS2tcpip.h>

#include "LockFreeQueue.h"
#include "RingBuffer.h"
#include "SPacket.h"

#pragma comment(lib, "ws2_32.lib")

using SessionID = unsigned __int64;
using namespace MyDataStructure;

#define ID_BIT 47
#define GET_SESSION_INDEX(ptr) (((ptr) >> ID_BIT) & 0x1ffff)
#define GET_SESSION_ID(ptr) ((ptr) & (0x00007fffffffffff))

union SessionFlag
{
	struct
	{
		short _releaseFlag;
		short _useCount;
	};
	long _flag;
};

class Session
{
public:
	Session();
	~Session();

public:
	void Initialize(SessionID id, SOCKET sock, SOCKADDR_IN addr);
	void Release(void);

public:
	SessionID _id;
	bool _bDisconnected;

	long _sendFlag;
	long _sendCount;

	SOCKET _sock;
	SOCKADDR_IN _addr;
	RingBuffer _recvBuf;
	LockFreeQueue<SPacket*> _sendPacketBuf;
	LockFreeQueue<SPacket*> _sendPacketWaitBuf;

	WSAOVERLAPPED _recvOvl;
	WSAOVERLAPPED _sendOvl;
	WSAOVERLAPPED _releaseOvl;

	SessionFlag _flag;
};