#pragma once

#include <WinSock2.h>

struct SendOverlapped : public WSAOVERLAPPED
{

};

struct RecvOverlapped : public WSAOVERLAPPED
{

};

struct Session
{
	SOCKET _clientSocket;
	char _buf[512];
	WSABUF _wsabuf;
	int sendByte;
	int recvByte;
	SendOverlapped _sendOverlapped;
	RecvOverlapped _recvOverlapped;
};