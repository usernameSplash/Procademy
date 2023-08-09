#include "Network.h"
#include "Session.h"

#include <cstdio>
#include <process.h>

namespace NetworkLibrary
{

	Network::Network(IContents* contents)
		: _contents(contents)
	{
		WSADATA wsa;

		_bShutdown = false;

		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			_bShutdown = true;
			return;
		}

		int bindRet;
		int listenRet;

		_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_listenSocket == INVALID_SOCKET)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			_bShutdown = true;
			return;
		}

		ZeroMemory(&_serverAddr, sizeof(_serverAddr));
		_serverAddr.sin_family = AF_INET;
		_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		_serverAddr.sin_port = htons(SERVER_PORT);

		bindRet = bind(_listenSocket, (SOCKADDR*)&_serverAddr, sizeof(_serverAddr));
		if (bindRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			_bShutdown = true;
			return;
		}
		wprintf(L"# Bind OK - Port : %d\n", SERVER_PORT);

		listenRet = listen(_listenSocket, SOMAXCONN);
		if (listenRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			_bShutdown = true;
			return;
		}
		wprintf(L"# Listen OK\n");

		_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if (_iocp == NULL)
		{
			wprintf(L"Error : IOCP is NULL\n");
			_bShutdown = true;
			return;
		}
		wprintf(L"# Create Completion Port OK\n");

		_acceptThread = (HANDLE)_beginthreadex(nullptr, 0, AcceptProc, (void*)this, 0, nullptr);
		if (_acceptThread == NULL)
		{
			wprintf(L"Error : Create Accept Thread Failed\n");
			_bShutdown = true;
			return;
		}
		wprintf(L"# Create Accept Thread OK\n");

		for (int iCnt = 0; iCnt < 16; ++iCnt)
		{
			_workerThreads[iCnt] = (HANDLE)_beginthreadex(nullptr, 0, WorkerProc, (void*)this, 0, nullptr);
			if (_acceptThread == NULL)
			{
				wprintf(L"Error : Create Worker Thread Failed\n");
				_bShutdown = true;
				return;
			}
		}
		wprintf(L"# Create Worker Thread OK\n");

		_contents->BindNetworkLibrary(this);

		wprintf(L"# Network Setup OK\n");

		return;
	}

	Network::~Network()
	{

	}

	unsigned int WINAPI Network::AcceptProc(void* arg)
	{
		Network* instance = static_cast<Network*>(arg);

		SOCKET clientSocket;
		SOCKADDR_IN clientAddr;
		int addrSize = sizeof(clientAddr);

		linger l;
		l.l_onoff = 1;
		l.l_linger = 0;

		int setSockOptRet;

		while (!instance->_bShutdown)
		{
			clientSocket = accept(instance->_listenSocket, (SOCKADDR*)&clientAddr, &addrSize);
			if (clientSocket == INVALID_SOCKET)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"Accept Error : %d on %d\n", errorCode, __LINE__);
				continue;
			}

			wprintf(L"# [Connected] Client %d.%d.%d.%d:%d\n", clientAddr.sin_addr.S_un.S_un_b.s_b1
				, clientAddr.sin_addr.S_un.S_un_b.s_b2
				, clientAddr.sin_addr.S_un.S_un_b.s_b3
				, clientAddr.sin_addr.S_un.S_un_b.s_b4
				, ntohs(clientAddr.sin_port)
			);

			setSockOptRet = setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l)); // RST Bit
			if (setSockOptRet == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
				closesocket(clientSocket);
				continue;
			}

			//create session;
			Session* session = new Session(instance->_sessionId, clientSocket, clientAddr);

			CreateIoCompletionPort((HANDLE)clientSocket, instance->_iocp, (ULONG_PTR)session, 0);

			// insert session into session list
			AcquireSRWLockExclusive(&instance->_sessionManager._lock);
			instance->_sessionManager.Insert(instance->_sessionId, session);
			ReleaseSRWLockExclusive(&instance->_sessionManager._lock);

			instance->RecvPost(session);
		}

		return 0;
	}

	unsigned int WINAPI Network::WorkerProc(void* arg)
	{
		Network* instance = static_cast<Network*>(arg);
		HANDLE iocp = instance->_iocp;

		int gqcsRet;

		while (1)
		{
			Session* session;
			MyOverlapped* overlapped;
			DWORD transferredByte;

			gqcsRet = GetQueuedCompletionStatus(iocp, &transferredByte, (PULONG_PTR)&session, (LPOVERLAPPED*)&overlapped, INFINITE);

			if (gqcsRet == 0 || transferredByte == 0)
			{
				session->_bNoMoreIO = true;

				if (gqcsRet == 0)
				{
					DWORD temp1, temp2;
					WSAGetOverlappedResult(session->_clientSocket, (LPWSAOVERLAPPED)overlapped, &temp1, FALSE, &temp2);
					
					int errorCode;
					errorCode = WSAGetLastError();
					wprintf(L"GQCS Error : %d on %d\n", errorCode, __LINE__);
				}
			}
			else if (overlapped->_type == eOverlappedType::SEND)
			{
				instance->SendHandler(session->_id, transferredByte);
			}
			else if (overlapped->_type == eOverlappedType::RECV)
			{
				instance->SendPost(session);
				if (!session->_bNoMoreIO)
				{
					instance->RecvPost(session);
				}
				continue;
			}

			if (InterlockedDecrement(&session->_ioCount) == 0)
			{
				instance->ReleaseSession(session);
			}
		}
	}

	void Network::ReleaseSession(Session* session)
	{
		AcquireSRWLockExclusive(&_sessionManager._lock);
		auto iter = _sessionManager._sessionMap.find(session->_id);
		if (iter == _sessionManager._sessionMap.end())
		{
			wprintf(L"Session Not Found in Session Map : ReleaseSession\n");
			ReleaseSRWLockExclusive(&_sessionManager._lock);
			return;
		}

		_sessionManager._sessionMap.erase(iter);

		ReleaseSRWLockExclusive(&_sessionManager._lock);

		AcquireSRWLockExclusive(&session->_lock);
		ReleaseSRWLockExclusive(&session->_lock);

		size_t deletedId = session->_id;
		SOCKADDR_IN deletedAddr = session->_clientAddr;
		closesocket(session->_clientSocket);
		delete session;

		wprintf(L"[Session Released] ID : %zu | Address : %d.%d.%d.%d:%d\n", deletedId,
			deletedAddr.sin_addr.S_un.S_un_b.s_b1,
			deletedAddr.sin_addr.S_un.S_un_b.s_b2,
			deletedAddr.sin_addr.S_un.S_un_b.s_b3,
			deletedAddr.sin_addr.S_un.S_un_b.s_b4,
			deletedAddr.sin_port
		);
	}

	void Network::SendPost(Session* session)
	{
		if (InterlockedIncrement(&session->_sendStatus) > 1)
		{
			InterlockedDecrement(&session->_sendStatus);
			return;
		}
		InterlockedIncrement(&session->_ioCount);

		DWORD sendBytes;
		DWORD flag;
		int sendRet;

		sendBytes = 0;
		flag = 0;

		session->_wsaSendBuf[0].buf = session->_sendBuf.GetFrontBufferPtr();
		session->_wsaSendBuf[0].len = (ULONG)session->_sendBuf.DirectDequeueSize();
		session->_wsaSendBuf[1].buf = session->_sendBuf.GetBufferPtr();
		session->_wsaSendBuf[1].len = (ULONG)session->_sendBuf.Size() - session->_wsaSendBuf[1].len;

		ZeroMemory(&session->_sendOverlapped, sizeof(OVERLAPPED));
		sendRet = WSASend(session->_clientSocket, session->_wsaSendBuf, 2, &sendBytes, flag, &session->_sendOverlapped._obj, NULL);

		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != ERROR_IO_PENDING)
			{
				wprintf(L"SendPost Error : %d | Session Id :  %zu\n", errorCode, session->_id);
				return;
			}
		}

		if (session->_sendBuf.MoveFront(sendRet) != sendRet)
		{
			wprintf(L"Send RingBuffer Error | Session Id :  %zu\n", session->_id);
			_bShutdown = true;
			return;
		}

		return;
	}

	void Network::RecvPost(Session* session)
	{
		DWORD recvBytes;
		DWORD flag;
		int recvRet;
		
		recvBytes = 0;
		flag = 0;

		size_t freeSize = session->_recvBuf.Capacity() - session->_recvBuf.DirectEnqueueSize();

		session->_wsaRecvBuf[0].buf = session->_recvBuf.GetRearBufferPtr();
		session->_wsaRecvBuf[0].len = (ULONG)session->_recvBuf.DirectEnqueueSize();
		session->_wsaRecvBuf[1].buf = session->_recvBuf.GetBufferPtr();
		session->_wsaRecvBuf[1].len = (ULONG)freeSize - session->_wsaRecvBuf[0].len;

		ZeroMemory(&session->_recvOverlapped._obj, sizeof(OVERLAPPED));

		recvRet = WSARecv(session->_clientSocket, session->_wsaRecvBuf, 2, &recvBytes, &flag, &session->_recvOverlapped._obj, NULL);

		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();

			if (errorCode != ERROR_IO_PENDING)
			{
				wprintf(L"RecvPost Error : %d | Session Id :  %zu\n", errorCode, session->_id);
				return;
			}
		}

		return;
	}

	void Network::RecvHandler(const size_t sessionId, const DWORD byte)
	{
		AcquireSRWLockShared(&_sessionManager._lock);

		auto iter = _sessionManager._sessionMap.find(sessionId);
		if (iter == _sessionManager._sessionMap.end()) // impossible case
		{
			wprintf(L"Session Not Found in Session Map : RecvHandler\n");
			ReleaseSRWLockShared(&_sessionManager._lock);	
			return;
		}

		Session* session = iter->second;

		AcquireSRWLockExclusive(&session->_lock);
		ReleaseSRWLockShared(&_sessionManager._lock);

		RecvByteToMsg(session);
		SendPost(session);

		if(!session->_bNoMoreIO)
		{
			RecvPost(session);
		}

		ReleaseSRWLockExclusive(&session->_lock);
	}

	void Network::SendHandler(const size_t sessionId, const DWORD byte)
	{
		// to do
		AcquireSRWLockShared(&_sessionManager._lock);

		auto iter = _sessionManager._sessionMap.find(sessionId);
		if (iter == _sessionManager._sessionMap.end())
		{
			wprintf(L"Session Not Found in Session Map : SendHandler\n");
			ReleaseSRWLockShared(&_sessionManager._lock);
			return;
		}

		Session* session = iter->second;

		AcquireSRWLockExclusive(&session->_lock);
		ReleaseSRWLockShared(&_sessionManager._lock);

		if (--(session->_sendStatus) == 0)
		{
			if (!session->_bNoMoreIO)
			{
				SendPost(session);
			}
		}

		ReleaseSRWLockExclusive(&session->_lock);
	}

	void Network::RecvByteToMsg(Session* session)
	{
		while (true)
		{
			size_t size = session->_recvBuf.Size();
			
			SPacket::SPacketHeader header;
			SPacket packet;
			
			if (size < sizeof(header))
			{
				break;
			}

			session->_recvBuf.Peek((char*)&header, sizeof(header));

			if (size < sizeof(header) + header.len)
			{
				break;
			}

			session->_recvBuf.Dequeue(sizeof(header));
			session->_recvBuf.Peek(packet.GetPayloadPtr(), header.len);

			packet.SetHeaderData(header);
			packet.MoveReadPos(sizeof(header));
			packet.MoveWritePos(sizeof(packet));

			_contents->OnMessage(session->_id, &packet);
		}
	}

	void Network::SendMsgToByte(const size_t sessionId, SPacket* packet)
	{
		AcquireSRWLockShared(&_sessionManager._lock);
		
		auto iter = _sessionManager._sessionMap.find(sessionId);
		if (iter == _sessionManager._sessionMap.end()) // impossible case
		{
			wprintf(L"Session Not Found in Session Map : SendMsgToByte\n");
			ReleaseSRWLockShared(&_sessionManager._lock);
			return;
		}

		Session* session = iter->second;
		ReleaseSRWLockShared(&_sessionManager._lock);
		
		SPacket::SPacketHeader header;
		header.len = packet->Size();

		packet->SetHeaderData(header);
		session->_sendBuf.Enqueue(packet->GetBufferPtr(), packet->Size());
	}

	void IContents::BindNetworkLibrary(Network* instance)
	{
		_network = instance;
	}
}
