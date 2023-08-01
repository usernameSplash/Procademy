#include "Network.h"
#include "Session.h"

#include <cstdio>
#include <process.h>

Network::Network(void)
{
	WSADATA wsa;
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

	_bShutdown = false;

	_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	_acceptThread = (HANDLE)_beginthreadex(nullptr, 0, AcceptProc, (void*)this, 0, nullptr);
	return;
}

unsigned int WINAPI Network::AcceptProc(void* arg)
{
	Network* instance = static_cast<Network*>(arg);

	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);

	linger l;
	int setSockOptRet;

	int recvRet;
	DWORD recvByte;
	DWORD flags;

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

		l.l_onoff = 1;
		l.l_linger = 0;
		setSockOptRet = setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof(l)); // RST Bit
		if (setSockOptRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			closesocket(clientSocket);
			continue;
		}

		//create session;
		Session* session = new Session();
		session->_clientSocket = clientSocket;
		session->_wsabuf.buf = session->_buf;
		ZeroMemory(&session->_sendOverlapped, sizeof(session->_sendOverlapped));
		ZeroMemory(&session->_recvOverlapped, sizeof(session->_recvOverlapped));

		CreateIoCompletionPort((HANDLE)clientSocket, instance->_iocp, (ULONG_PTR)session, 0);

		flags = 0;
		recvRet = WSARecv(clientSocket, &session->_wsabuf, 1, &recvByte, &flags, &session->_recvOverlapped, nullptr);
		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != ERROR_IO_PENDING)
			{
				wprintf(L"WSARecv Error : %d on %d\n", errorCode, __LINE__);
				closesocket(clientSocket);
				delete session;
				continue;
			}
		}

		// insert session into session list
		// ...
	}

	return;
}