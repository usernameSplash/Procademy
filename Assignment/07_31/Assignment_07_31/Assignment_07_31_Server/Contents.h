#pragma once
#include "Network.h"

namespace NetworkLibrary
{
	class Contents : public IContents
	{
	public:
		virtual void OnAccept(size_t sessionId) override;
		virtual void OnMessage(size_t sessionId, SPacket* packet) override;
		virtual void OnRelease(size_t sessionId) override;
	};
}