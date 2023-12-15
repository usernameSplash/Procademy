#pragma once

#include "RingBuffer.h"

#include <WinSock2.h>

namespace Network
{
	using SessionID = __int64;

#define INVALID_SESSION_ID -1

	struct Session
	{
	public:
		Session()
			: _id(INVALID_SESSION_ID)
			, _socket(INVALID_SOCKET)
			, _sendPacketLen(0)
			, _sendPacketCnt(0)
			, _ioCount(0)
			, _sendFlag(0)
		{
			InitializeCriticalSection(&_lock);

			ZeroMemory(&_recvOvl, sizeof(OVERLAPPED));
			ZeroMemory(&_sendOvl, sizeof(OVERLAPPED));
			ZeroMemory(&_releaseOvl, sizeof(OVERLAPPED));
		}

		void SetSocket(const SOCKET socket)
		{
			_socket = socket;
		}

	public:
		CRITICAL_SECTION _lock;

		SessionID _id;
		SOCKET _socket;
		OVERLAPPED _recvOvl;
		OVERLAPPED _sendOvl;
		OVERLAPPED _releaseOvl;

		MyDataStructure::RingBuffer _recvBuf;
		MyDataStructure::RingBuffer _sendBuf;

		long _sendPacketLen;
		long _sendPacketCnt;

		long _ioCount;
		long _sendFlag;
	};
}