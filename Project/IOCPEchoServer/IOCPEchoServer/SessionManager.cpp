#include "SessionManager.h"

namespace Network
{
	SessionManager::SessionManager(long maxSessionCount)
		: _sessionCnt(0)
		, _freeIdCnt(maxSessionCount)
		, _maxSessionCount(maxSessionCount)
	{
		_sessionList = new Session[_maxSessionCount];
		_freeIdList.reserve(_maxSessionCount);

		for (int iCnt = _maxSessionCount - 1; iCnt >= 0; --iCnt)
		{
			_freeIdList.push_back(iCnt);
		}

		InitializeSRWLock(&_freeIdListLock);
	}

	SessionManager::~SessionManager()
	{
		delete[] _sessionList;
	}

	Session* SessionManager::AllocSession(void)
	{
		Session* newSession;
		SessionID newId;

		AcquireSRWLockExclusive(&_freeIdListLock);
		if (_freeIdList.empty())
		{
			ReleaseSRWLockExclusive(&_freeIdListLock);
			return nullptr; //Failed
		}
		newId = _freeIdList.back();
		_freeIdList.pop_back();
		ReleaseSRWLockExclusive(&_freeIdListLock);

		newSession = &_sessionList[newId];
		newSession->_id = newId;

		InterlockedIncrement(&_sessionCnt);

		return newSession;
	}

	void SessionManager::FreeSession(const SessionID sessionId)
	{
		_sessionList[sessionId]._id = INVALID_SESSION_ID;

		AcquireSRWLockExclusive(&_freeIdListLock);
		_freeIdList.push_back(sessionId);
		ReleaseSRWLockExclusive(&_freeIdListLock);

		InterlockedDecrement(&_sessionCnt);

		return;
	}
}