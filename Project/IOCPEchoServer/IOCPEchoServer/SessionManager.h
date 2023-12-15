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

		bool CanAllocNewSession(void)
		{
			return (_sessionCnt < _maxSessionCount);
		}
		long GetSessionCount(void)
		{
			return _sessionCnt;
		}

	public:
		Session* _sessionList;

	private:
		vector<SessionID> _freeIdList;
		SRWLOCK _freeIdListLock;

	private:
		int _freeIdCnt;
		long _maxSessionCount;
		long _sessionCnt;
	};

}