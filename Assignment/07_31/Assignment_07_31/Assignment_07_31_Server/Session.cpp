#include "Session.h"

namespace NetworkLibrary
{
	Session::Session(size_t id, SOCKET socket, SOCKADDR_IN addr)
		: _id(id)
		, _clientSocket(socket)
		, _clientAddr(addr)
		, _recvBuf(4096)
		, _sendBuf(4096)
		, _ioCount(1)
		, _sendStatus(0)
		, _bNoMoreIO(false)
	{
		ZeroMemory(&_sendOverlapped._obj, sizeof(OVERLAPPED));
		_sendOverlapped._type = eOverlappedType::SEND;

		ZeroMemory(&_recvOverlapped._obj, sizeof(OVERLAPPED));
		_recvOverlapped._type = eOverlappedType::RECV;

		InitializeSRWLock(&_lock);
	}

	SessionManager::SessionManager()
	{
		_sessionMap.reserve(1024);
		InitializeSRWLock(&_lock);
	}

	void SessionManager::Insert(size_t id, Session* session)
	{
		_sessionMap.insert(std::make_pair(id, session));
	}

	void SessionManager::Delete(size_t id)
	{
		_sessionMap.erase(id);
	}

	std::unordered_map<size_t, Session*>::iterator SessionManager::Find(size_t id)
	{
		auto result = _sessionMap.find(id);

		return result;
	}
}