#pragma once

#include "Protocol.h"

using PlayerID = unsigned __int64;

class Player
{
public:
	Player()
		: _regionX(-1)
		, _regionY(-1)
	{
		_lastRecvTime = GetTickCount64();
	};

public:
	unsigned __int64 _sessionId;
	PlayerID _playerId;
	
	// Initialize Once in Login
	__int64 _accountNumber;
	wchar_t _id[ID_LEN_MAX];
	wchar_t _nickname[NICKNAME_LEN_MAX];
	char _sessionKey[SESSION_KEY_LEN_MAX];

	short _regionX;
	short _regionY;
	ULONGLONG _lastRecvTime;
};