#pragma once

#include <Windows.h>

#define LOG_ARRAY_LEN 2000000
#define FAIL_LOG_ARRAY_LEN 2000

#define JOB_ENQ_CAS1 0xe1
#define JOB_ENQ_CAS2 0xe2
#define JOB_ENQ_CAS2_FAIL 0xef
#define JOB_ENQ_CAS3 0xe3
#define JOB_ENQ_CAS3_FAIL 0xe3f
#define JOB_ENQ_BEFORE_LOOP 0xeb

#define JOB_DEQ_CAS 0xd1
#define JOB_DEQ_CAS_FAIL 0xdf
#define JOB_DEQ_NEXT_NULL 0xde
#define JOB_DEQ_BEFORE_LOOP 0xdb

struct LogData
{
	LONG64 _idx;
	LONG64 _threadId;
	LONG64 _jobType;
	__int64 _exchangePtr;
	__int64 _comparePtr;
	__int64 _realPtr;
	__int64 _size;
};

extern __int64 g_enqCnt;
extern __int64 g_deqCnt;

extern __int64 g_logIndex;
extern LogData g_logArray[];

extern __int64 g_casFailLogIndex;
extern LogData g_casFailLogArray[];

inline void Log(LONG64 jobType, __int64 exchangePtr, __int64 comparePtr, __int64 realPtr, __int64 size)
{
	LONG64 idx = InterlockedIncrement64(&g_logIndex);
	
	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
	g_logArray[idx % LOG_ARRAY_LEN]._jobType = jobType;
	g_logArray[idx % LOG_ARRAY_LEN]._exchangePtr = exchangePtr;
	g_logArray[idx % LOG_ARRAY_LEN]._comparePtr = comparePtr;
	g_logArray[idx % LOG_ARRAY_LEN]._realPtr = realPtr;
	g_logArray[idx % LOG_ARRAY_LEN]._size = size;

	if (jobType == JOB_ENQ_CAS2_FAIL)
	{
		LONG64 failIdx = InterlockedIncrement64(&g_casFailLogIndex);

		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._idx = idx;
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._threadId = GetCurrentThreadId();
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._jobType = jobType;
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._exchangePtr = exchangePtr;
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._comparePtr = comparePtr;
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._realPtr = realPtr;
		g_casFailLogArray[failIdx % FAIL_LOG_ARRAY_LEN]._size = size;

		__debugbreak();
	}
}