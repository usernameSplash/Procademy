#pragma once

#include <Windows.h>

#define LOG_ARRAY_LEN 100000
#define JOB_PUSH 0
#define JOB_POP -1

struct LogData
{
	LONG64 _idx;
	LONG64 _threadId;
	LONG64 _jobType;
	void* _nodePtr;
	void* _nextPtr;
};

extern __int64 g_pushCnt;
extern __int64 g_popCnt;

extern __int64 g_logIndex;
extern LogData g_logArray[];