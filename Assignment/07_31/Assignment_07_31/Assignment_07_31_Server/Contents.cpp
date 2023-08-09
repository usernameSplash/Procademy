#include "Contents.h"

namespace NetworkLibrary
{
	void Contents::OnAccept(size_t sessionId)
	{
		// to do
	}

	void Contents::OnMessage(size_t sessionId, SPacket* packet)
	{
		__int64 value;
		(*packet) >> value;

		wprintf(L"[Message From %zu] : %lld\n", sessionId, value);

		SPacket sendPacket;
		sendPacket << value;

		_network->SendMsgToByte(sessionId, &sendPacket);
	}
	
	void Contents::OnRelease(size_t sessionId)
	{
		// to do
	}
}