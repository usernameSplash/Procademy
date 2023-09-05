#pragma once

#include <WinSock2.h>
#include <unordered_map>

#include "RingBuffer.h"
#include "SerializationBuffer.h"

using SessionID = __int64;

using namespace std;
using namespace MyDataStructure;

#define WSA_SEND_BUF_LEN_MAX 100

namespace TCPFighter_IOCP_Server
{
	enum class eOverlappedType
	{
		SEND = 0,
		RECV = 1,
	};

	struct MyOverlapped
	{
		OVERLAPPED _obj;
		eOverlappedType _type;
	};

	struct Session
	{
	public:
		Session();

	public:
		void Initialize(const size_t id, const SOCKET socket, const SOCKADDR_IN addr);

	public:
		SessionID _id;
		SOCKET _clientSocket;
		SOCKADDR_IN _clientAddr;

		MyOverlapped _sendOverlapped;
		MyOverlapped _recvOverlapped;

		RingBuffer _recvBuf;
		RingBuffer _sendBuf;

		DWORD _sendPacketLen;
		DWORD _sendPacketCnt;

		SRWLOCK _lock;
		long _ioCount;
		long _sendStatus;

		bool _bConnected;
	};

	class SessionManager : public std::unordered_map<size_t, Session*>
	{
	public:
		SessionManager();
	};
}