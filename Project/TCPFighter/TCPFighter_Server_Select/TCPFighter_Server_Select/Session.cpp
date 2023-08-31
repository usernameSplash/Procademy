#include "Session.h"

namespace TCPFighter_Select_Server
{
	void Session::Initialize(int id)
	{
		_id = id;

		_bRunning = true;
		_bReleased = false;

		_recvBuf.ClearBuffer();
		_sendBuf.ClearBuffer();
		_recvPacket.Clear();
		_sendPacket.Clear();

		return;
	}
}