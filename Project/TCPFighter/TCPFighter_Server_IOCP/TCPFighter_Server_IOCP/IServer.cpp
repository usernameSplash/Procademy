
#include "IServer.h"
#include "main.h"

#include <cstring>
#include <cstdio>

namespace TCPFighter_IOCP_Server
{
	bool IServer::Start(const wchar_t* ip, const short port, const int numOfWorkerThread, const bool nagleOption, const int maxSession)
	{
		// Set Member Variable
		wcsncpy(_ip, ip, 16);
		_port = port;
		_numOfWorkerThread = numOfWorkerThread;
		_nagleOption = nagleOption;
		_maxSession = maxSession;

		if (numOfWorkerThread <= 0)
		{
			wprintf(L"# Invalid Argument : numOfWorkerThread must be bigger than 0.\n");
			return false;
		}

		int nagleOptSetRet;
		int sendBufSizeSetRet;
		int bindRet;
		int listenRet;

		// Initialize Winsock Library;
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			wprintf(L"# WSAStartrUp Error\n");
			return false;
		}

		_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_listenSocket == INVALID_SOCKET)
		{
			wprintf(L"# Create Listen Socket Failed\n");
			return false;
		}

		if (nagleOption)
		{
			linger l;
			l.l_onoff = 1;
			l.l_linger = 0;

			nagleOptSetRet = setsockopt(_listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
			if (nagleOptSetRet == SOCKET_ERROR)
			{
				wprintf(L"# Nagle Option Setting Failed\n");
				return false;
			}
		}

		{
			int sendBufSize = 0;

			sendBufSizeSetRet = setsockopt(_listenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));
			if (nagleOptSetRet == SOCKET_ERROR)
			{
				wprintf(L"# Socket Send Buffer Size Setting Failed\n");
				return false;
			}
		}

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(serverAddr));

		serverAddr.sin_family = AF_INET;
		InetPtonW(AF_INET, ip, &serverAddr.sin_addr.s_addr);
		serverAddr.sin_port = htons(port);

		bindRet = bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (bindRet == SOCKET_ERROR)
		{
			wprintf(L"# Listen Socket Binding Error\n");
			return false;
		}

		listenRet = listen(_listenSocket, SOMAXCONN);
		if (listenRet == SOCKET_ERROR)
		{
			wprintf(L"# Listen Failed\n");
			return false;
		}

		// Create IOCP
		_releaseIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		if (_releaseIOCP == NULL)
		{
			wprintf(L"# Release IOCP Create Failed\n");
			return false;
		}

		_networkIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (_networkIOCP == NULL)
		{
			wprintf(L"# Network IOCP Create Failed\n");
			return false;
		}

		// Cretae Thread
		_releaseThread = (HANDLE)_beginthreadex(nullptr, 0, ReleaseProc, this, 0, nullptr);
		if (_releaseThread == NULL)
		{
			wprintf(L"# Release Thread Beginning Failed\n");
			return false;
		}

		_acceptThread = (HANDLE)_beginthreadex(nullptr, 0, AcceptProc, this, 0, nullptr);
		if (_acceptThread == NULL)
		{
			wprintf(L"# Accept Thread Beginning Failed\n");
			return false;
		}

		_workerThreads = new HANDLE[numOfWorkerThread];
		for (int iCnt = 0; iCnt < numOfWorkerThread; ++iCnt)
		{
			_workerThreads[iCnt] = (HANDLE)_beginthreadex(nullptr, 0, WorkerProc, this, 0, nullptr);
			if (_workerThreads[iCnt] == NULL)
			{
				wprintf(L"# Worker Thread %d Beginning Failed\n", iCnt);
				return false;
			}
		}

		_sessionPool = new ObjectPool<Session>(SESSION_MAX);
		_sendPacketPool = new ObjectPool<SPacket>(PAKCET_POOL_SIZE);

		_sessionManager.reserve(10000);
		for (int sessionCnt = 0; sessionCnt < SESSION_MAX; ++sessionCnt)
		{
			_sessionManager.insert(make_pair(sessionCnt, nullptr));
		}

		_returnedIds.reserve(SESSION_MAX);
		InitializeSRWLock(&_returnedIdListLock);
		_returnedIdCnt = 0;

		wprintf(L"# Network Server Setup Success\n");

		return true;
	}

	void IServer::Shutdown(void)
	{
		if (_bRunning == false)
		{
			wprintf(L"Server is Terminated Already.\n");
			return;
		}

		_bRunning = false;

		// No More New Session
		closesocket(_listenSocket);

		// No More IO Handling
		PostQueuedCompletionStatus(_releaseIOCP, 0, 0, 0);

		for (int iCnt = 0; iCnt < _numOfWorkerThread; ++iCnt)
		{
			PostQueuedCompletionStatus(_networkIOCP, 0, 0, 0);
		}

		WaitForSingleObject(_releaseThread, INFINITE);
		WaitForSingleObject(_acceptThread, INFINITE);
		WaitForMultipleObjects(16, _workerThreads, true, INFINITE);

		// Close All Client Sockets
		for (auto iter = _sessionManager.begin(); iter != _sessionManager.end(); ++iter)
		{
			Session* session = iter->second;
			if (session == nullptr)
			{
				continue;
			}

			closesocket(session->_clientSocket);
			_sessionPool->Free(session);
		}

		// Close All Kernel Objects
		WSACleanup();

		CloseHandle(_releaseIOCP);
		CloseHandle(_networkIOCP);
		CloseHandle(_releaseThread);
		CloseHandle(_acceptThread);
		
		for (int iCnt = 0; iCnt < _numOfWorkerThread; ++iCnt)
		{
			CloseHandle(_workerThreads[iCnt]);
		}

		delete[] _workerThreads;
		delete _sessionPool;

		wprintf(L"# Server Shutdown Complete\n");

		return;
	}

	bool IServer::Disconnect(const SessionID sessionId)
	{
		Session* session = _sessionManager[sessionId];
		
		if (session == nullptr)
		{
			return;
		}

		AcquireSRWLockShared(&session->_lock);
		
		// Make Session's ioCount to Zero By 2 times of PQCS Call.
		session->_bConnected = false;
		PostQueuedCompletionStatus(_networkIOCP, 0, (ULONG_PTR)session, 0);
		PostQueuedCompletionStatus(_networkIOCP, 0, (ULONG_PTR)session, 0);

		ReleaseSRWLockShared(&session->_lock);

		return true;
	}

	bool IServer::SendPacket(const SessionID sessionId, SPacket* packet)
	{
		Session* session = _sessionManager[sessionId];

		if (session == nullptr)
		{
			return;
		}
		
		AcquireSRWLockShared(&session->_lock);

		SPacketHeader header;
		header.len = static_cast<short>(packet->Size());
		packet->SetHeaderData(&header);

		session->_sendBuf.Enqueue((char*)&packet, sizeof(SPacket*));		// Enqueue Address Value (SPacket* Type)
		session->_sendPacketCnt++;

		SendPost(session);

		ReleaseSRWLockShared(&session->_lock);

		return true;
	}

	void IServer::UpdateMonitorStatistics(void)
	{
		_acceptTPS = _acceptCnt;
		_disconnectTPS = _disconnectCnt;
		_recvTPS = _recvCnt;
		_sendTPS = _sendCnt;

		_acceptTotal += _acceptTPS;
		_disconnectTotal += _disconnectTPS;

		_acceptCnt = 0;
		_disconnectCnt = 0;
		_recvCnt = 0;
		_sendCnt = 0;
	}

	unsigned int WINAPI IServer::AcceptProc(void* arg)
	{
		IServer* instance = static_cast<IServer*>(arg);

		SessionID sessionIdProvider = 0;

		wprintf(L"# Accept Proc Start\n");

		int addrLen = sizeof(SOCKADDR_IN);

		while (true)
		{
			SOCKET clientSocket;
			SOCKADDR_IN clientAddr;

			clientSocket = accept(instance->_listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

			if (clientSocket == INVALID_SOCKET)
			{
				wprintf(L"# Accept Failed\n");
				g_bRunning = false;
				break;
			}

			if (!g_bRunning || !instance->_bRunning)
			{
				break;
			}

			if (sessionIdProvider >= SESSION_MAX)
			{
				if (instance->_returnedIdCnt == 0)
				{
					closesocket(clientSocket);
					int temp = 0;
					WaitOnAddress(&instance->_returnedIdCnt, &temp, sizeof(instance->_returnedIdCnt), INFINITE);
					continue;
				}
			}

#pragma region validate_connection			
			wchar_t clientIp[16];
			short port;

			InetNtopW(AF_INET, &clientAddr.sin_addr.s_addr, clientIp, sizeof(clientIp));
			port = ntohs(clientAddr.sin_port);

			if (instance->OnConnectionRequest(clientIp, port) == false)
			{
				closesocket(clientSocket); // block invalid connection request
				continue;
			}
#pragma endregion

			SessionID newId;
			if (sessionIdProvider >= SESSION_MAX)
			{
				AcquireSRWLockExclusive(&instance->_returnedIdListLock);
				newId = instance->_returnedIds.back();
				instance->_returnedIds.pop_back();
				instance->_returnedIdCnt--;
				ReleaseSRWLockExclusive(&instance->_returnedIdListLock);
			}
			else
			{
				newId = sessionIdProvider++;
			}

			Session* session = instance->_sessionPool->Alloc();
			session->Initialize(newId, clientSocket, clientAddr);

			//AcquireSRWLockExclusive(&instance->_sessionManager._lock);
			instance->_sessionManager[sessionIdProvider] = session;
			//ReleaseSRWLockExclusive(&instance->_sessionManager._lock);

			InterlockedIncrement(&instance->_sessionCnt);
			++instance->_acceptCnt;
			instance->OnClientJoin(session->_id);

			CreateIoCompletionPort((HANDLE)clientSocket, instance->_networkIOCP, (ULONG_PTR)session, 0);
			
			instance->RecvPost(session);
		}

		wprintf(L"# Accept Proc End\n");

		return 0;
	}

	unsigned int WINAPI IServer::WorkerProc(void* arg)
	{
		IServer* instance = static_cast<IServer*>(arg);

		while (true)
		{
			Session* session;
			DWORD transferredBytes;
			MyOverlapped* overlapped;

			int gqcsRet = GetQueuedCompletionStatus(instance->_networkIOCP, &transferredBytes, (PULONG_PTR)&session, (LPOVERLAPPED*)&overlapped, INFINITE);

			if (!g_bRunning || !instance->_bRunning)
			{
				break;
			}

			if (gqcsRet == 0 || transferredBytes == 0)
			{
				if (gqcsRet == 0)
				{
					DWORD temp1, temp2;
					WSAGetOverlappedResult(session->_clientSocket, (LPWSAOVERLAPPED)&overlapped, &temp1, FALSE, &temp2);
					
					int errorCode;
					errorCode = WSAGetLastError();

					switch (errorCode)
					{
					case WSAECONNABORTED:
					case WSAECONNRESET:
						break;
					default:
						wprintf(L"GQCS Error : %d on %d\n", errorCode, __LINE__);
						break;
					}
				}
			}
			else if (overlapped->_type == eOverlappedType::SEND)
			{
				InterlockedIncrement((long*)&instance->_sendCnt);
				instance->SendHandler(session, transferredBytes);
			}
			else if (overlapped->_type == eOverlappedType::RECV)
			{
				InterlockedIncrement((long*)&instance->_recvCnt);
				instance->RecvHandler(session, transferredBytes);
			}

			if (InterlockedDecrement(&session->_ioCount) == 0)
			{
				PostQueuedCompletionStatus(instance->_releaseIOCP, 0, (ULONG_PTR)session, 0);
			}
		}
	}

	unsigned int WINAPI IServer::ReleaseProc(void* arg)
	{
		IServer* instance = static_cast<IServer*>(arg);

		wprintf(L"# Release Proc Start\n");

		while (true)
		{
			Session* session;
			DWORD transferredBytes;
			OVERLAPPED* overlapped;

			int gqcsRet = GetQueuedCompletionStatus(instance->_releaseIOCP, &transferredBytes, (PULONG_PTR)&session, &overlapped, INFINITE);

			if (!g_bRunning || !instance->_bRunning)
			{
				break;
			}

			SessionID releaseId = session->_id;
			instance->_sessionManager[releaseId] = nullptr;

			AcquireSRWLockExclusive(&session->_lock);
			ReleaseSRWLockExclusive(&session->_lock);

			closesocket(session->_clientSocket);

			SessionID clientId = session->_id;
			instance->_sessionPool->Free(session);
			instance->_disconnectCnt++;

			InterlockedDecrement(&instance->_sessionCnt);

			AcquireSRWLockExclusive(&instance->_returnedIdListLock);
			instance->_returnedIds.push_back(clientId);
			instance->_returnedIdCnt++;
			ReleaseSRWLockExclusive(&instance->_returnedIdListLock);
			WakeByAddressSingle(&instance->_returnedIdCnt);

			instance->OnClientLeave(clientId);
		}

		wprintf(L"# Release Proc End\n");
	}

	void IServer::RecvPost(Session* session)
	{
		if (!session->_bConnected)
		{
			return;
		}

		DWORD flag;
		DWORD recvBytes;
		int recvRet;

		flag = 0;
		recvBytes = 0;

		size_t freeSize = session->_recvBuf.Capacity() - session->_recvBuf.Size();

		WSABUF wsaRecvBuf[2];

		wsaRecvBuf[0].buf = session->_recvBuf.GetRearBufferPtr();
		wsaRecvBuf[0].len = (ULONG)session->_recvBuf.DirectEnqueueSize();
		wsaRecvBuf[1].buf = session->_recvBuf.GetBufferPtr();
		wsaRecvBuf[1].len = (ULONG)freeSize - wsaRecvBuf[0].len;

		ZeroMemory(&session->_recvOverlapped._obj, sizeof(OVERLAPPED));
		recvRet = WSARecv(session->_clientSocket, wsaRecvBuf, 2, &recvBytes, &flag, &session->_recvOverlapped._obj, NULL);

		if (recvRet == SOCKET_ERROR)
		{
			int errorCode;
			errorCode = WSAGetLastError();

			switch (errorCode)
			{
			case ERROR_IO_PENDING:
			case WSAECONNABORTED:
			case WSAECONNRESET:
				break;
			default:
				wprintf(L"# Session(%lld) : RecvPost Error : %d on %d\n", session->_id, errorCode, __LINE__);
				Disconnect(session->_id);
				break;
			}
		}
	}

	void IServer::SendPost(Session* session)
	{
		if (!session->_bConnected)
		{
			return;
		}

		if (session->_sendPacketCnt == 0)
		{
			return;
		}

		if (InterlockedExchange(&session->_sendStatus, 1) == 1)
		{
			return;
		}

		InterlockedIncrement(&session->_ioCount);

		if (session->_sendPacketCnt == 0)
		{
			session->_sendStatus = 0;
			PostQueuedCompletionStatus(_networkIOCP, 0, (ULONG_PTR)session, (LPOVERLAPPED)&session->_sendOverlapped);
			return;
		}

		session->_sendPacketLen = session->_sendPacketCnt;
		session->_sendPacketCnt = 0;

		DWORD flag;
		DWORD sendBytes;
		int sendRet;

		flag = 0;

		if (WSA_SEND_BUF_LEN_MAX < session->_sendPacketLen)
		{
			wprintf(L"# Error : Send Packet is Too Many\n");
			Disconnect(session->_id);
		}

		WSABUF sendWSABuf[WSA_SEND_BUF_LEN_MAX];
		SPacket* packets[WSA_SEND_BUF_LEN_MAX];

		session->_sendBuf.Peek((char*)packets, session->_sendBuf.Size());

		for (int packetCnt = 0; packetCnt < session->_sendPacketLen; ++packetCnt)
		{
			SPacketHeader header;
			packets[packetCnt]->GetHeaderData(&header);

			sendWSABuf[packetCnt].buf = packets[packetCnt]->GetBufferPtr();
			sendWSABuf[packetCnt].len = sizeof(SPacketHeader) + header.len;
		}

		ZeroMemory(&session->_sendOverlapped, sizeof(OVERLAPPED));
		sendRet = WSASend(session->_clientSocket, sendWSABuf, session->_sendPacketLen, &sendBytes, flag, &session->_sendOverlapped._obj, NULL);

		if (sendRet == SOCKET_ERROR)
		{
			int errorCode;
			errorCode = WSAGetLastError();

			switch (errorCode)
			{
			case ERROR_IO_PENDING:
			case WSAECONNABORTED:
			case WSAECONNRESET:
				break;
			default:
				wprintf(L"# Session(%lld) : SendPost Error : %d on %d\n", session->_id, errorCode, __LINE__);
				Disconnect(session->_id);
				break;
			}
		}
	}

	void IServer::RecvHandler(Session* session, const DWORD byte)
	{
		AcquireSRWLockExclusive(&session->_lock);

		session->_recvBuf.MoveRear(byte);
		RecvPost(session);

		// Make Received Data to a Message
		while (true)
		{
			size_t size = session->_recvBuf.Size();

			SPacketHeader header;
			SPacket packet(64);

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
			session->_recvBuf.Dequeue(header.len);

			packet.SetHeaderData(&header);
			packet.MoveWritePos(header.len);

			OnRecv(session->_id, &packet);
		}

		RecvPost(session);

		ReleaseSRWLockExclusive(&session->_lock);

		return;
	}

	void IServer::SendHandler(Session* session, const DWORD byte)
	{
		AcquireSRWLockExclusive(&session->_lock);

		SPacket* packetList[WSA_SEND_BUF_LEN_MAX];
		session->_sendBuf.Peek((char*)&packetList, sizeof(SPacket*) * session->_sendPacketLen);
		session->_sendBuf.Dequeue(sizeof(SPacket*) * session->_sendPacketLen);

		session->_sendStatus = 0;
		SendPost(session);

		ReleaseSRWLockExclusive(&session->_lock);

		for (int packetCnt = 0; packetCnt < session->_sendPacketLen; packetCnt++)
		{
			_sendPacketPool->Free(packetList[packetCnt]);
		}

		return;
	}
}