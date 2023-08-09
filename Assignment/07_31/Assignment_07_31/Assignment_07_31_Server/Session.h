#pragma once

#include <WinSock2.h>
#include <unordered_map>

#include "RingBuffer.h"

using namespace MyDataStructure;

namespace NetworkLibrary
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
		Session() = default;
		Session(size_t id, SOCKET socket, SOCKADDR_IN addr);

	public:
		size_t _id;
		SOCKET _clientSocket;
		SOCKADDR_IN _clientAddr;

		MyOverlapped _sendOverlapped;
		MyOverlapped _recvOverlapped;

		RingBuffer _sendBuf;
		RingBuffer _recvBuf;
		WSABUF _wsaSendBuf[2];
		WSABUF _wsaRecvBuf[2];

		SRWLOCK _lock;
		long _ioCount;
		long _sendStatus;

		bool _bNoMoreIO;
	};

	class SessionManager
	{
	public:
		SessionManager();
		std::unordered_map<size_t, Session*> _sessionMap;

	public:
		void Insert(size_t id, Session* session);
		void Delete(size_t id);
		std::unordered_map<size_t, Session*>::iterator Find(size_t id);

	public:
		SRWLOCK _lock;

	};
}