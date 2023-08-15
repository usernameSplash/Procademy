#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 30000

#define PACKET_LEN 10

class EchoClient
{
public:
	EchoClient();
	~EchoClient();

public:
	bool IsRunning(void);
	void Shutdown(void);
	void Terminate(void);

private:
	static unsigned int WINAPI SendProc(void* arg);
	static unsigned int WINAPI RecvProc(void* arg);

private:
	SOCKET _socket;
	SOCKADDR_IN _addr;
	HANDLE _sendThread;
	HANDLE _recvThread;

	bool _bRunning;
	bool _bShutdown;
};