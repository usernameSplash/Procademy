#include "INetServer.h"

#ifdef SERVER_NET

#include <cstdio>

INetServer::INetServer() {}

INetServer::~INetServer() {}

bool INetServer::ServerInitialize(const wchar_t* ip, const short port, const int threadNum, const bool nagle)
{
	wcsncpy(_ip, ip, INET_ADDRSTRLEN);
	_port = port;
	_threadNum = threadNum;
	_nagle = nagle;

	WSADATA wsa;
	int wsaStartUpRet = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (wsaStartUpRet != 0)
	{
		wprintf(L"# WSAStartUp Error\n");
		return false;
	}

	_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_listenSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# Listen Socket Error : %d\n", errorCode);
		return false;
	}

	/* Socket Option */

	linger l;
	l.l_onoff = 1;
	l.l_linger = 0;
	
	int setLingerRet = setsockopt(_listenSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l));
	if (setLingerRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# Linger Opt Setting Error : %d", errorCode);
		return false;
	}

	int sendBufSize = 0;
	int setSendBuf = setsockopt(_listenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));
	if (setSendBuf == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# SendBuf Size Setting Error : %d", errorCode);
		return false;
	}

	int nagleOpt;
	if (_nagle)
	{
		nagleOpt = FALSE;
	}
	else
	{
		nagleOpt = TRUE;
	}

	int setNagleRet = setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&nagleOpt, sizeof(nagleOpt));
	if (setNagleRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# Nagle Opt Setting Error : %d", errorCode);
		return false;
	}

	/* Socket Bind */

	SOCKADDR_IN serverAddr;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_port);

	int bindRet = bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (bindRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# Socket Bind Error : %d", errorCode);
		return false;
	}

	/* Listen */

	int listenRet = listen(_listenSocket, SOMAXCONN_HINT(SOMAXCONN));
	if (listenRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"# Socket Listen Error : %d", errorCode);
		return false;
	}

	/* Session List Initialize */

	for (int iCnt = 0; iCnt < SESSION_MAX; ++iCnt)
	{
		_freeIdx.Push(iCnt);
		_sessions[iCnt] = new Session;
	}

	/* IOCP */

	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 8);
	if (_iocp == NULL)
	{
		wprintf(L"# IOCP Create Error\n");
		return false;
	}

	_acceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptProc, this, 0, NULL);
	if (_acceptThread == NULL)
	{
		wprintf(L"# Create Accept Thread Error\n");
		return false;
	}

	_iocpWorkerThreads = new HANDLE[_threadNum];
	for (int iCnt = 0; iCnt < _threadNum; ++iCnt)
	{
		_iocpWorkerThreads[iCnt] = (HANDLE)_beginthreadex(NULL, 0, IOCPWorkerProc, this, 0, NULL);
		if (_iocpWorkerThreads[iCnt] == NULL)
		{
			wprintf(L"# Create IOCP Worker Thread %d Error\n", iCnt);
			return false;
		}
	}

	_isAlive = 1;
	OnInitialize();

	return true;
}

bool INetServer::ServerTerminate(void)
{
	if (InterlockedExchange(&_isAlive, 0) != 1)
	{
		wprintf(L"# Server is Terminated Already.\n");
		return false;
	}

	closesocket(_listenSocket);

	for (int iCnt = 0; iCnt < _threadNum; ++iCnt)
	{
		PostQueuedCompletionStatus(_iocp, 0, 0, 0);
	}
	
	WaitForMultipleObjects(_threadNum, _iocpWorkerThreads, TRUE, INFINITE);
	WaitForSingleObject(_acceptThread, INFINITE);

	for (int iCnt = 0; iCnt < SESSION_MAX; ++iCnt)
	{
		Session* session = _sessions[iCnt];
		closesocket(session->_sock);
		delete session;
	}

	WSACleanup();

	CloseHandle(_iocp);
	CloseHandle(_acceptThread);
	for (int iCnt = 0; iCnt < _threadNum; ++iCnt)
	{
		CloseHandle(_iocpWorkerThreads[iCnt]);
	}
	delete[] _iocpWorkerThreads;

	OnTerminate();

	return true;
}

bool INetServer::SendPacket(const SessionID sessionId, SPacket* packet)
{
	Session* session = TrySessionAccess(sessionId, __LINE__);

	if (session == nullptr)
	{
		return false;
	}

	if (packet->mIsEncoded == 0)
	{
		PacketHeader header;
		short len = (short)packet->GetPayloadSize();

		header._code = PACKET_CODE;
		header._len = len;
		header._randKey = rand() % 256;
		packet->Encode(header);

		packet->SetHeaderData(&header);
	}
	
	session->_sendPacketBuf.Enqueue(packet);
	SendPost(session);

	ReturnSessionAccess(session, __LINE__);

	return true;
}

bool INetServer::Disconnect(const SessionID sessionId)
{
	Session* session = TrySessionAccess(sessionId, __LINE__);
	
	if (session == nullptr)
	{
		return false;
	}

	session->_bDisconnected = true;
	CancelIoEx((HANDLE)session->_sock, (LPOVERLAPPED)&session->_recvOvl);
	CancelIoEx((HANDLE)session->_sock, (LPOVERLAPPED)&session->_sendOvl);
	ReturnSessionAccess(session, __LINE__);

	return true;
}

unsigned int WINAPI INetServer::AcceptProc(void* arg)
{
	INetServer* instance = (INetServer*)arg;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);

	while (true)
	{
		SOCKET clientSocket = accept(instance->_listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"# Accept Error : %d\n", errorCode);
			break;
		}

		if (instance->_isAlive == 0)
		{
			break;
		}

		instance->_acceptCnt++;

		if (instance->_sessionCnt > SESSION_MAX)
		{
			closesocket(clientSocket);
			InterlockedIncrement(&instance->_disconnectCnt);
			wprintf(L"# Session Limits Exceed\n");
			continue;
		}

		if (instance->_freeIdx.Size() == 0)
		{
			closesocket(clientSocket);
			InterlockedIncrement(&instance->_disconnectCnt);
			wprintf(L"# No Free Session List Index\n");
			continue;
		}
		
		wchar_t ip[INET_ADDRSTRLEN];
		InetNtopW(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);

		bool isValidRequest = instance->OnConnectRequest(ip, ntohs(clientAddr.sin_port));
		if (isValidRequest == false)
		{
			// Block Invalid Connection;
		}

		InterlockedIncrement(&instance->_sessionCnt);

		SessionID newId = InterlockedIncrement64(&instance->_sessionIdProvider);
		int newIdx = instance->_freeIdx.Pop();
		__int64 newIdxShifted = (__int64)newIdx << ID_BIT;

		newId = newId | newIdxShifted;

		Session* newSession = instance->_sessions[newIdx];

		newSession->Initialize(newId, clientSocket, clientAddr);

		CreateIoCompletionPort((HANDLE)clientSocket, instance->_iocp, (ULONG_PTR)newSession, 0);
		
		instance->OnAccept(newId);
		instance->RecvPost(newSession);
	}

	return 0;
}

unsigned int WINAPI INetServer::IOCPWorkerProc(void* arg)
{
	INetServer* instance = (INetServer*)arg;
	DWORD threadId = GetCurrentThreadId();

	wprintf(L"# IOCP Worker Thread %d Start\n", threadId);

	while (true)
	{
		Session* session;
		DWORD bytes;
		OVERLAPPED* ovl;
		
		int gqcsRet = GetQueuedCompletionStatus((HANDLE)instance->_iocp, &bytes, (PULONG_PTR)&session, &ovl, INFINITE);

		if (instance->_isAlive == 0)
		{
			break;
		}

		if (ovl == &session->_releaseOvl)
		{
			instance->ReleaseHandler(session);
			continue;
		}

		if (instance->TrySessionAccess(session->_id, __LINE__) == nullptr)
		{
			continue;
		}

		if (gqcsRet == 0 || bytes == 0)
		{
			if (gqcsRet == 0)
			{
				int errorCode = WSAGetLastError();
				if (errorCode != WSAEINTR && errorCode != WSAECONNABORTED 
					&& errorCode != WSAECONNRESET && errorCode != WSAENOTSOCK 
					&& errorCode != WSA_OPERATION_ABORTED && errorCode != ERROR_NETNAME_DELETED)
				{
					wprintf(L"# GQCS Error : %d\n", errorCode);
				}
			}
		}
		else if (ovl == &session->_recvOvl)
		{
			bool ret = instance->RecvHandler(session, bytes);
		}
		else if (ovl == &session->_sendOvl)
		{
			instance->SendHandler(session, bytes);
		}

		instance->DecrementSessionUseCount(session, __LINE__); // To Decrement Use Count Increased By Recv/Send Post;
		instance->ReturnSessionAccess(session, __LINE__); // To Release Session Access in Worker Thread;
	}
	return 0;
}

bool INetServer::RecvPost(Session* session)
{
	DWORD recvBytes = 0;
	DWORD flags = 0;
	WSABUF wsaBuf[2];

	int freeSize = (int)(session->_recvBuf.Capacity() - session->_recvBuf.Size());

	wsaBuf[0].buf = session->_recvBuf.GetRearBufferPtr();
	wsaBuf[0].len = (ULONG)session->_recvBuf.DirectEnqueueSize();
	wsaBuf[1].buf = session->_recvBuf.GetBufferPtr();
	wsaBuf[1].len = freeSize - wsaBuf[0].len;

	ZeroMemory(&session->_recvOvl, sizeof(session->_recvOvl));

	if (session->_bDisconnected == true)
	{
		return false;
	}

	IncrementSessionUseCount(session, __LINE__);

	int recvRet = WSARecv(session->_sock, wsaBuf, RECV_MAX, &recvBytes, &flags, (LPWSAOVERLAPPED)&session->_recvOvl, NULL);

	if (recvRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		
		if (errorCode != WSA_IO_PENDING)
		{
			if (errorCode != WSAEINTR && errorCode != WSAECONNABORTED && errorCode != WSAECONNRESET)
			{
				wprintf(L"# RecvPost Error : %d\n", errorCode);
			}
			DecrementSessionUseCount(session, __LINE__);
			return false;
		}
		else if (session->_bDisconnected == true)
		{
			CancelIoEx((HANDLE)session->_sock, &session->_recvOvl);
			return false;
		}
	}

	return true;
}

bool INetServer::SendPost(Session* session)
{
	if (session->_sendPacketBuf.Size() == 0)
	{
		return false;
	}

	if (InterlockedExchange(&session->_sendFlag, 1) == 1)
	{
		return false;
	}

	if (session->_sendPacketBuf.Size() == 0)
	{
		return false;
	}

	WSABUF wsaBuf[SEND_MAX];
	int wsaBufIdx = 0;
	int packetNum = (int)session->_sendPacketBuf.Size();

	for (; wsaBufIdx < packetNum; ++wsaBufIdx)
	{
		if (wsaBufIdx >= SEND_MAX)
		{
			break;
		}

		PacketHeader header;
		SPacket* packet = session->_sendPacketBuf.Dequeue();
		if (packet == nullptr)
		{
			break;
		}
		packet->GetHeaderData(&header);
		if (header._code != PACKET_CODE)
		{
			wprintf(L"Session %lld Code Error : %d, len : %d\n", GET_PTR(session->_id), header._code, header._len);
		}

		wsaBuf[wsaBufIdx].buf = packet->GetBufferPtr();
		wsaBuf[wsaBufIdx].len = (ULONG)packet->Size();
		session->_sendPacketWaitBuf.Enqueue(packet);
	}

	session->_sendCount = wsaBufIdx;

	DWORD sendBytes;
	ZeroMemory(&session->_sendOvl, sizeof(session->_sendOvl));

	if (session->_bDisconnected == true)
	{
		InterlockedExchange(&session->_sendFlag, 0);
		return false;
	}

	IncrementSessionUseCount(session, __LINE__);

	int sendRet = WSASend(session->_sock, wsaBuf, wsaBufIdx, &sendBytes, 0, (LPWSAOVERLAPPED)&session->_sendOvl, NULL);

	if (sendRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			if (errorCode != WSAEINTR && errorCode != WSAECONNABORTED && errorCode != WSAECONNRESET)
			{
				wprintf(L"# SendPost Error : %d\n", errorCode);
			}
			InterlockedExchange(&session->_sendFlag, 0);
			DecrementSessionUseCount(session, __LINE__);
			return false;
		}
		else if (session->_bDisconnected == true)
		{
			CancelIoEx((HANDLE)session->_sock, &session->_sendOvl);
			InterlockedExchange(&session->_sendFlag, 0);
			return false;
		}
	}

	InterlockedIncrement(&_sendCnt);
	return true;
}

bool INetServer::RecvHandler(Session* session, const int byte)
{
	int moveRet = (int)session->_recvBuf.MoveRear(byte);
	if (moveRet != byte)
	{
		Disconnect(session->_id);
		wprintf(L"# %lld Recv Buffer Error\n", session->_id);
		return false;
	}

	int size = (int)session->_recvBuf.Size();

	while (true)
	{
		if (size <= HEADER_SIZE)
		{
			break;
		}

		PacketHeader header;
		session->_recvBuf.Peek((char*)&header, HEADER_SIZE);
		//wprintf(L"[RECV] Session ID : %lld : Packet Code : %d, Packet Len : %d\n", GET_PTR(session->_id), header._code, header._len);

		if (header._code != PACKET_CODE)
		{
			Disconnect(session->_id);
			return false;
		}

		if (size < HEADER_SIZE + header._len)
		{
			break;
		}

		SPacket* recvPacket = SPacket::Alloc();
		recvPacket->AddUseCnt(1);

		session->_recvBuf.Dequeue(HEADER_SIZE);
		session->_recvBuf.Peek(recvPacket->GetPayloadPtr(), header._len);

		if (recvPacket->Decode(header) == false)
		{
			Disconnect(session->_id);
			SPacket::Free(recvPacket);
			return false;
		}

		session->_recvBuf.Dequeue(header._len);
		recvPacket->MoveWritePos(header._len);

		//short* ptr = (short*)recvPacket->GetPayloadPtr();
		//wprintf(L"[TYPE] RecvHandler sessionID : %lld, type : %d, packet ptr : %p\n", GET_PTR(session->_id), *ptr, recvPacket);
		
		OnRecv(session->_id, recvPacket);
		SPacket::Free(recvPacket);

		InterlockedIncrement(&_recvCnt);
		size -= (HEADER_SIZE + header._len);
	}

	bool reRecvRet = RecvPost(session);

	return reRecvRet;
}

bool INetServer::SendHandler(Session* session, const int byte)
{
	for (int iCnt = 0; iCnt < session->_sendCount; ++iCnt)
	{
		SPacket* packet = session->_sendPacketWaitBuf.Dequeue();
		if (packet == NULL)
		{
			break;
		}

		SPacket::Free(packet);
	}

	//OnSend(session->_id, byte);
	InterlockedExchange(&session->_sendFlag, 0);

	bool reSendRet = SendPost(session);

	return reSendRet;
}

void INetServer::ReleaseHandler(Session* session)
{
	SessionID id = session->_id;
	int idx = GET_SESSION_INDEX(id);
	SOCKET clientSocket = session->_sock;

	session->Release();

	closesocket(clientSocket);

	_freeIdx.Push(idx);
	InterlockedIncrement(&_disconnectCnt);
	InterlockedDecrement(&_sessionCnt);

	OnRelease(id);
}


Session* INetServer::TrySessionAccess(const SessionID sessionId, int line)
{
	__int64 index = GET_SESSION_INDEX(sessionId);

	if (index >= SESSION_MAX)
	{
		return nullptr;
	}

	Session* session = _sessions[index];

	IncrementSessionUseCount(session, line);

	if (session->_flag._releaseFlag == 1)
	{
		DecrementSessionUseCount(session, line);
		return nullptr;
	}

	if (session->_id != sessionId)
	{
		DecrementSessionUseCount(session, line);
		return nullptr; // for debug
	}

	return session;
}

void INetServer::ReturnSessionAccess(Session* session, int line)
{
	DecrementSessionUseCount(session, line);
}

void INetServer::IncrementSessionUseCount(Session* session, int line)
{
	short useCount = InterlockedIncrement16(&session->_flag._useCount);
	//wprintf(L"# Increment Session %lld Line : %d, Use Count : %d\n", session->_id, line, useCount);
}

void INetServer::DecrementSessionUseCount(Session* session, int line)
{
	short useCount = InterlockedDecrement16(&session->_flag._useCount);
	//wprintf(L"# Decrement Session %lld Line : %d, Use Count : %d\n", session->_id, line, useCount);

	SessionFlag tempFlag;
	tempFlag._useCount = 0;
	tempFlag._releaseFlag = 1;

	if (useCount == 0)
	{
		if (InterlockedCompareExchange(&session->_flag._flag, tempFlag._flag, 0) == 0)
		{
			PostQueuedCompletionStatus(_iocp, 1, (ULONG_PTR)session, (LPOVERLAPPED)&session->_releaseOvl);
		}
	}

	return;
}

#endif