#include "Session.h"

Session::Session()
{
	_flag._flag = 0;
}

Session::~Session()
{

}

void Session::Initialize(SessionID id, SOCKET sock, SOCKADDR_IN addr)
{
	InterlockedExchange(&_sendFlag, 0);
	InterlockedExchange16(&_flag._releaseFlag, 0);

	_id = id;
	_bDisconnected = false;

	_sock = sock;
	_addr = addr;

	ZeroMemory(&_recvOvl, sizeof(WSAOVERLAPPED));
	ZeroMemory(&_sendOvl, sizeof(WSAOVERLAPPED));
	ZeroMemory(&_releaseOvl, sizeof(WSAOVERLAPPED));
}

void Session::Release(void)
{
	_bDisconnected = true;
	_sock = INVALID_SOCKET;
	_recvBuf.ClearBuffer();


	// TODO : sendBuf Clear
	//while(_sendPacketBuf)

	//while(_sendPacketWaitBuf)
}
