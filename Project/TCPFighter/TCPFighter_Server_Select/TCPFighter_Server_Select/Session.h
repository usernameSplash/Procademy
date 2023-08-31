#pragma once

#include "RingBuffer.h"
#include "Protocol.h"
#include "SerializationBuffer.h"

using namespace MyDataStructure;

namespace TCPFighter_Select_Server
{
	using SessionID = int;

	struct Session
	{
	public:
		Session() = default;
		~Session() = default;

	public:
		void Initialize(int id);

	public:
		bool _bRunning;
		bool _bReleased;

		SessionID _id;

		SOCKET _sock;
		SOCKADDR_IN _addr;

		RingBuffer _recvBuf;
		RingBuffer _sendBuf;

		SPacket _recvPacket;
		SPacket _sendPacket;

		ULONGLONG _lastRecvTime;
	};
}