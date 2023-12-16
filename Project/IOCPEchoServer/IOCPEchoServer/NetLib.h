#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <vector>

#include "ObjectPool.h"
#include "Session.h"
#include "SessionManager.h"
#include "SerializationBuffer.h"

using namespace std;

namespace Network
{
	class NetLib
	{
	protected:
		bool Start(const wchar_t* ip, const short port, const int numOfWorkerThread, const bool nagleOption, const bool noSendBufOption, const int maxSession);

	public:
		void Shutdown(void);

	protected:
		bool Disconnect(const SessionID sessionId);
		bool SendPacket(const SessionID sessionId, SPacket* packet);

	protected:
		// Monitoring Statistics Data
		int GetAcceptTotal(void) const
		{
			return _acceptTotal;
		}
		int GetDisconnectTotal(void) const
		{
			return _disconnectTotal;
		}

		int GetAcceptTPS(void) const
		{
			return _acceptTPS;
		}

		int GetDisconnectTPS(void) const
		{
			return _disconnectTPS;
		}

		int GetRecvTPS(void) const
		{
			return _recvTPS;
		}

		int GetSendTPS(void) const
		{
			return _sendTPS;
		}

		int GetSessionCount(void)
		{
			return (int)_sessionManager.GetSessionCount();
		}

		void UpdateMonitorStatistics(void);

	protected:
		// pure virtual fucntions : reactions for network IO
		// don't call in derived class
		virtual bool OnConnectionRequest(const wchar_t* ip, const short port) = 0;
		virtual void OnClientJoin(const SessionID sessionId) = 0;
		virtual void OnClientLeave(const SessionID sessionId) = 0;
		virtual void OnRecv(const SessionID sessionId, SPacket* packet) = 0;
		virtual void OnSend(const SessionID sessionId) = 0;
		virtual void OnError(int errorCode, const wchar_t* errorMsg) = 0;

	private:
		// Network Core Threads
		static unsigned int WINAPI AcceptProc(void* arg);
		static unsigned int WINAPI WorkerProc(void* arg);

	private:
		// WSARecv, WSASend Wrapper
		void RecvPost(Session* session);
		void SendPost(Session* session);

	private:
		// Handler Functions, called by IOCP Worker Thread (WorkerProc)
		void RecvHandler(Session* session, const DWORD byte);
		void SendHandler(Session* session, const DWORD byte);
		void ReleaseHandler(Session* session);

	private:
		// Server Infomation
		wchar_t _ip[16];
		short _port;
		int _numOfWorkerThread;
		bool _nagleOption;
		int _maxSession;

		SessionManager _sessionManager;

	protected:
		ObjectPool<SPacket>* _sendPacketPool;

	private:
		// Server State
		bool _bRunning;
		// SessionMap

		long _acceptTotal;
		long _disconnectTotal;

		long _acceptTPS;
		long _disconnectTPS;
		long _recvTPS;
		long _sendTPS;

		long _acceptCnt;
		long _disconnectCnt;
		long _recvCnt;
		long _sendCnt;

	private:
		// Kernel Object Handles
		SOCKET _listenSocket;

		HANDLE _networkIOCP;

		HANDLE _acceptThread;
		HANDLE* _workerThreads;
	};
}