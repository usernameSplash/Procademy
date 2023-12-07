#pragma once

#include "Session.h"

#include <Windows.h>
#include <vector>

using namespace std;

namespace Network
{
	class SessionManager
	{
	public:
		SessionManager(long maxSessionCount);
		~SessionManager();

		Session* AllocSession(void);
		void FreeSession(const SessionID sessionId);

	public:
		Session* _sessionList;
		vector<SessionID> _freeIdList;

		SRWLOCK _freeIdListLock;

	private:
		int _freeIdCnt;
		long _maxSessionCount;
	};

}