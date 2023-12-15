#include "NetLib.h"
#include "main.h"

namespace Network
{
	bool NetLib::Start(const wchar_t* ip, const short port, const int numOfWorkerThread, const bool nagleOption, const bool noSendBufOption, const int maxSession)
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

		if (noSendBufOption)
		{
			int sendBufSize = 0;

			sendBufSizeSetRet = setsockopt(_listenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufSize, sizeof(sendBufSize));
			if (sendBufSizeSetRet == SOCKET_ERROR)
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
		_networkIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (_networkIOCP == NULL)
		{
			wprintf(L"# Network IOCP Create Failed\n");
			return false;
		}

		// Cretae Thread

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

		//_sendPacketPool = new ObjectPool<SPacket>(PAKCET_POOL_SIZE);

		wprintf(L"# Network Server Setup Success\n");

		return true;
	}

	void NetLib::Shutdown(void)
	{
		if (_bRunning == false)
		{
			wprintf(L"# Server is terminated already\n");
			return;
		}

		// No More New Session
		closesocket(_listenSocket);

		// No More IO Handling
		for (int iCnt = 0; iCnt < _numOfWorkerThread; ++iCnt)
		{
			PostQueuedCompletionStatus(_networkIOCP, 0, 0, 0);
		}

		WaitForSingleObject(_acceptThread, INFINITE);
		WaitForMultipleObjects(16, _workerThreads, true, INFINITE);

		// Close All Client Sockets
		for (int iCnt = 0; iCnt < _maxSession; ++iCnt)
		{
			Session session = _sessionManager._sessionList[iCnt];
			closesocket(session._socket);
		}

		// Close All Kernel Objects
		WSACleanup();

		CloseHandle(_networkIOCP);
		CloseHandle(_acceptThread);

		for (int iCnt = 0; iCnt < _numOfWorkerThread; ++iCnt)
		{
			CloseHandle(_workerThreads[iCnt]);
		}

		delete[] _workerThreads;

		wprintf(L"# Server Shutdown Complete\n");

		return;
	}

	bool NetLib::Disconnect(const SessionID sessionId)
	{
		// To do
	}

	bool NetLib::SendPacket(const SessionID sessionId, SPacket* packet)
	{

	}

	unsigned int WINAPI NetLib::AcceptProc(void* arg)
	{
		NetLib* instance = static_cast<NetLib*>(arg);

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

			if (!instance->_sessionManager.CanAllocNewSession())
			{
				wprintf(L"# Limit Of Session Num Exceeded\n");
				closesocket(clientSocket);
			}

#pragma region validate_connection
			wchar_t clientIp[16];
			short port;
			InetNtopW(AF_INET, &clientAddr.sin_addr, clientIp, 16);
			port = htons(clientAddr.sin_port);

			if (instance->OnConnectionRequest(clientIp, port))
			{
				wprintf(L"# Invalid Connection, [IP : %s:%d]", clientIp, port);
				closesocket(clientSocket);
				continue;
			}
#pragma endregion

#pragma region new_session_allocation
			Session* newSession = instance->_sessionManager.AllocSession();

			if (newSession == nullptr)
			{
				wprintf(L"# Session Allocation Failed\n");
				closesocket(clientSocket);
				continue;
			}

			newSession->SetSocket(clientSocket);
			++instance->_acceptCnt;

			instance->OnClientJoin(newSession->_id);

			CreateIoCompletionPort((HANDLE)clientSocket, instance->_networkIOCP, (ULONG_PTR)newSession, 0);

			instance->RecvPost(newSession);

#pragma endregion
		}

		wprintf(L"# Accept Proc End\n");

		return 0;
	}

	unsigned int WINAPI NetLib::WorkerProc(void* arg)
	{
		NetLib* instance = static_cast<NetLib*>(arg);

		wprintf(L"# Worker Proc Start\n");

		while (true)
		{
			Session* session;
			DWORD transferredBytes;
			OVERLAPPED* ovl;

			int gqcsRet = GetQueuedCompletionStatus(instance->_networkIOCP, &transferredBytes, (PULONG_PTR)&session, (LPOVERLAPPED*)&ovl, INFINITE);

			if (!g_bRunning || !instance->_bRunning)
			{
				break;
			}

			if (ovl == &session->_releaseOvl)
			{
				instance->ReleaseHandler(session);
				continue;
			}

			if (gqcsRet == 0 || transferredBytes == 0)
			{
				if (gqcsRet == 0)
				{
					DWORD temp1, temp2;

					WSAGetOverlappedResult(session->_socket, (LPWSAOVERLAPPED)ovl, &temp1, FALSE, &temp2);

					int errorCode;
					errorCode = WSAGetLastError();

					switch (errorCode)
					{
					case WSAENOTSOCK:
					case WSAECONNABORTED:
					case WSAECONNRESET:
						break;
					default:
						wprintf(L"GQCS Error : %d on %d\n", errorCode, __LINE__);
						break;
					}
				}
			}

			else if (ovl == &session->_recvOvl)
			{
				instance->RecvHandler(session, transferredBytes);
				InterlockedIncrement(&instance->_recvCnt);
			}

			else if (ovl == &session->_sendOvl)
			{
				instance->SendHandler(session, transferredBytes);
				InterlockedIncrement(&instance->_sendCnt);
			}

			if (InterlockedDecrement(&session->_ioCount) == 0)
			{
				PostQueuedCompletionStatus(instance->_networkIOCP, 1, (ULONG_PTR)session, (LPOVERLAPPED)&session->_releaseOvl);
			}
		}

		wprintf(L"# Worker Proc End\n");

		return 0;
	}

	void NetLib::RecvPost(Session* session)
	{

	}

	void NetLib::SendPost(Session* session)
	{

	}

	void NetLib::RecvHandler(Session* session, const DWORD byte)
	{

	}

	void NetLib::SendHandler(Session* session, const DWORD byte)
	{

	}

	void NetLib::ReleaseHandler(Session* session)
	{

	}
}