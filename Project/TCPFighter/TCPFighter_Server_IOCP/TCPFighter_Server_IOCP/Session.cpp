#include "Session.h"

namespace TCPFighter_IOCP_Server
{
	Session::Session()
		: _id(ULLONG_MAX) // invalid ID
		, _clientSocket(INVALID_SOCKET)
		, _recvBuf(4096)
		, _sendBuf(4096)
		, _ioCount(1)
		, _sendStatus(0)
		, _bConnected(false)
		, _bNoMoreIO(false)
	{
		ZeroMemory(&_clientAddr, sizeof(_clientAddr));

		ZeroMemory(&_sendOverlapped._obj, sizeof(OVERLAPPED));
		_sendOverlapped._type = eOverlappedType::SEND;

		ZeroMemory(&_recvOverlapped._obj, sizeof(OVERLAPPED));
		_recvOverlapped._type = eOverlappedType::RECV;

		InitializeSRWLock(&_lock);
	}

	void Session::Initialize(const size_t id, const SOCKET socket, const SOCKADDR_IN addr)
	{
		_id = id;
		_clientSocket = socket;
		_clientAddr = addr;
	}

	SessionManager::SessionManager()
	{
		//InitializeSRWLock(&_lock);
	}
}