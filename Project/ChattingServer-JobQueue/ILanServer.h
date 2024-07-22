#pragma once

#ifdef SERVER_LAN

#include "ServerConfig.h"
#include "Session.h"
#include "SPacket.h"
#include "LockFreeStack.h"

#include <WS2tcpip.h>
#include <process.h>

class ILanServer
{
protected:
	ILanServer();
	virtual ~ILanServer();

	// Server Core Function
protected:
	bool ServerInitialize(const wchar_t* ip, const short port, const int threadNum, const bool nagle);
	bool Terminate(void);

	bool SendPacket(const SessionID sessionId, SPacket* packet);
	bool Disconnect(const SessionID sessionId);

	// Pure Virtual Function
protected:
	virtual void OnInitialize(void) = 0;
	virtual void OnTerminate(void) = 0;

	virtual bool OnConnectRequest(const wchar_t* ip, const short port) = 0;
	virtual void OnAccept(const SessionID sessionId) = 0;
	virtual void OnRelease(const SessionID sessionId) = 0;

	virtual void OnRecv(const SessionID sessionId, SPacket* packet) = 0;
	virtual void OnSend(const SessionID sessionId, const int sentByte) = 0;

	// Sub Thread Function
private:
	static unsigned int WINAPI AcceptProc(void* arg);
	static unsigned int WINAPI IOCPWorkerProc(void* arg);

	// WSARecv & WSASend Wrapper
private:
	void RecvPost(Session* session);
	void SendPost(Session* session);

	// Network Handler Function
private:
	void RecvHandler(Session* session, const int byte);
	void SendHandler(Session* session, const int byte);
	void ReleaseHandler(Session* session);


	// Monitoring Function
protected:
	inline void UpdateMonitorData(void)
	{
		long acceptCnt = InterlockedExchange(&_acceptCnt, 0);
		long disconnectCnt = InterlockedExchange(&_disconnectCnt, 0);
		long recvCnt = InterlockedExchange(&_recvCnt, 0);
		long sendCnt = InterlockedExchange(&_sendCnt, 0);

		_acceptTPS = acceptCnt;
		_disconnectTPS = disconnectCnt;
		_recvTPS = recvCnt;
		_sendTPS = sendCnt;

		_acceptTotal += acceptCnt;
		_disconnectTotal = disconnectCnt;
	}

	inline long GetAcceptTotal(void)
	{
		return _acceptTotal;
	}

	inline long GetDisconnectTotal(void)
	{
		return _disconnectTotal;
	}

	inline long GetAcceptTPS(void)
	{
		return _acceptTPS;
	}

	inline long GetDisconnectTPS(void)
	{
		return _disconnectTPS;
	}

	inline long GetRecvTPS(void)
	{
		return _recvTPS;
	}

	inline long GetSendTPS(void)
	{
		return _sendTPS;
	}

	inline long GetSessionCnt(void)
	{
		return _sessionCnt;
	}

	// Be Set By Initializer Arguments
private:
	wchar_t _ip[INET_ADDRSTRLEN];
	short _port;
	bool _nagle;
	int _threadNum;

private:
	SOCKET _listenSocket;
	long _isAlive = 0;

	// Windows Kernel Object
private:
	HANDLE _acceptThread;
	HANDLE* _iocpWorkerThreads;
	HANDLE _iocp;

	// Managing Session
private:
	Session* _sessions[SESSION_MAX];
	LockFreeStack<int> _freeIdx;
	long _sessionCnt = 0;
	__int64 _sessionIdProvider = 0;

	inline __int64 GetSessionIndex(__int64 key)
	{
		return (((key) >> ID_BIT) & 0x1ffff);
	}

	inline __int64 GetSessionId(__int64 key)
	{
		return (key & 0x00007fffffffffff);
	}

private:
	// Monitoring Data
	long _acceptTotal = 0;
	long _disconnectTotal = 0;

	long _acceptTPS = 0;
	long _disconnectTPS = 0;
	long _recvTPS = 0;
	long _sendTPS = 0;

	long _acceptCnt = 0;
	long _disconnectCnt = 0;
	long _recvCnt = 0;
	long _sendCnt = 0;
};

#endif