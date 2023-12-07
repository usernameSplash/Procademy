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

	}

	bool NetLib::SendPacket(const SessionID sessionId, SPacket* packet)
	{

	}

	unsigned int WINAPI NetLib::AcceptProc(void* arg)
	{

	}

	unsigned int WINAPI NetLib::WorkerProc(void* arg)
	{

	}

	void NetLib::RecvPost(Session* session)
	{

	}
}