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
		{
			InitializeSRWLock(&_lock);
			_id = INVALID_SESSION_ID;
			_ioCount = 0;
			_sendFlag = 0;
		}

		void SetSocket(const SOCKET socket)
		{
			_socket = socket;
		}

	public:
		SessionID _id;

		SOCKET _socket;
		OVERLAPPED _recvOvl;
		OVERLAPPED _sendOvl;

		long _ioCount;
		long _sendFlag;

		SRWLOCK _lock;
	};
}