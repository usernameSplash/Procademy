#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#include <WinSock2.h>
#include <unordered_map>

#define SERVER_PORT 30000

class Network
{
private:
	Network(void);

public:
	static Network* GetInstance(void);

private:
	static unsigned int WINAPI AcceptProc(void* arg);
	static unsigned int WINAPI WorkerProc(void* arg);

public:
	bool _bShutdown;

	SOCKET _listenSocket;
	SOCKADDR_IN _serverAddr;

	HANDLE _iocp;
	HANDLE _acceptThread;
	HANDLE _workerThreads[16];
};
