#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>

#include "SerializationBuffer.h"
#include "Session.h"

using namespace std;

namespace TCPFighter_IOCP_Server
{
	class IServer
	{
	protected:
		bool Start(const wchar_t* ip, const short port, const int numOfWorkerThread, const bool nagleOption, const int maxSession);

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

		int GetSessionCount(void) const
		{
			return _sessionCnt;
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
		static unsigned int WINAPI ReleaseProc(void* arg);

	private:
		// WSARecv, WSASend Wrapper
		void RecvPost(Session* session);
		void SendPost(Session* session);

	private:
		// Handler Functions, called by IOCP Worker Thread (WorkerProc)
		void RecvHandler(Session* session, const DWORD byte);
		void SendHandler(Session* session, const DWORD byte);

	private:
		// Server Infomation
		wchar_t _ip[16];
		short _port;
		int _numOfWorkerThread;
		bool _nagleOption;
		int _maxSession;

		SessionManager _sessionManager;

	private:
		// Server State
		bool _bRunning;
		long _sessionCnt;
		// SessionMap

		int _acceptTotal;
		int _disconnectTotal;

		int _acceptTPS;
		int _disconnectTPS;
		int _recvTPS;
		int _sendTPS;

		int _acceptCnt;
		int _disconnectCnt;
		int _recvCnt;
		int _sendCnt;

	private:
		// Kernel Object Handles
		SOCKET _listenSocket;

		HANDLE _releaseIOCP;
		HANDLE _networkIOCP;

		HANDLE _releaseThread;
		HANDLE _acceptThread;
		HANDLE* _workerThreads;
	};
}