#pragma once

class SPacket;

enum class eJobType
{
	SYSTEM = 0,
	CONTENTS,
	INVALID,
};

enum class eSystemJobType
{
	ACCEPT,
	RELEASE,
	TIMEOUT,
	TERMINATE,
	INVALID,
};

class Job
{
public:
	Job()
	{

	};

	void Initialize(const eJobType jobType, const eSystemJobType systemJobType, const SessionID sessionId, SPacket* packet)
	{
		_jobType = jobType;
		_systemJobType = systemJobType;
		_sessionId = sessionId;
		_packet = packet;
	}

public:
	eJobType _jobType;
	eSystemJobType _systemJobType;
	unsigned __int64 _sessionId;
	SPacket* _packet;
};