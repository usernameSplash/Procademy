#include "EchoClient.h"

#include <WS2tcpip.h>
#include <cstdio>
#include <process.h>
#include "SerializationBuffer.h"

EchoClient::EchoClient()
{
	WSAData wsa;

	_bShutdown = false;
	_bRunning = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		_bShutdown = true;
		return;
	}

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		_bShutdown = true;
		return;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(SERVER_PORT);

	int connectRet;
	connectRet = connect(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (connectRet == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
		_bShutdown = true;
		return;
	}

	wprintf(L"# Connect OK\n");

	_sendThread = (HANDLE)_beginthreadex(nullptr, 0, SendProc, (void*)this, 0, nullptr);
	_recvThread = (HANDLE)_beginthreadex(nullptr, 0, RecvProc, (void*)this, 0, nullptr);

	_bRunning = true;

	wprintf(L"# Echo Client Setup OK\n");
}

EchoClient::~EchoClient()
{

}

bool EchoClient::IsRunning(void)
{
	return _bRunning;
}

void EchoClient::Shutdown(void)
{
	_bShutdown = true;
	_bRunning = false;
}

void EchoClient::Terminate(void)
{
	closesocket(_socket);
	WaitForSingleObject(_sendThread, INFINITE);
	WaitForSingleObject(_recvThread, INFINITE);

	return;
}

unsigned int WINAPI EchoClient::SendProc(void* arg)
{
	EchoClient* instance = static_cast<EchoClient*>(arg);
	
	int sendRet;

	__int64 value = 0;

	wprintf(L"# Send Thread Start\n");

	while (!instance->_bShutdown)
	{
		SPacket packet;
		SPacketHeader header;

		packet << value;

		header.len = (short)packet.Size();

		packet.SetHeaderData(&header);

		sendRet = send(instance->_socket, packet.GetBufferPtr(), PACKET_LEN, 0);
		if (sendRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			instance->Shutdown();
			break;
		}

		wprintf(L"Send Data : %d\n", value);
		value++;
		
		Sleep(100);
	}

	wprintf(L"# Send Thread End\n");

	return 0;
}

unsigned int WINAPI EchoClient::RecvProc(void* arg)
{
	EchoClient* instance = static_cast<EchoClient*>(arg);

	int recvRet;
	
	__int64 value = 0;
	__int64 recvValue;

	wprintf(L"# Recv Thread Start\n");

	while (!instance->_bShutdown)
	{
		SPacket packet;
		SPacketHeader header;

		recvRet = recv(instance->_socket, packet.GetBufferPtr(), PACKET_LEN, 0);
		if (recvRet == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			wprintf(L"Error : %d on %d\n", errorCode, __LINE__);
			instance->Shutdown();
			break;
		}

		packet.GetHeaderData(&header);
		packet.MoveWritePos(header.len);

		if (header.len != 8)
		{
			wprintf(L"Recv Header Size is Wrong. Received Value : %d\n", header.len);
			break;
		}

		packet >> recvValue;

		if (recvValue != value)
		{
			wprintf(L"Recv Value is Wrong. Expected Value is %lld. Received Value : %lld\n", value, recvValue);
			value++;
			continue;
		}

		wprintf(L"Recv Success %lld\n", recvValue);
		value++;
	}

	wprintf(L"# Recv Thread End\n");

	return 0;
}