#pragma once

#include <Windows.h>

#define LOG_ARRAY_LEN 100000

#define BEFORE_ENQ 0xbebebebe
#define WHILE_ENQ 0xeeeeeeee
#define AFTER_ENQ 0xaeaeaeae
#define BEFORE_DEQ 0xbdbdbdbd
#define WHILE_DEQ 0xdddddddd
#define AFTER_DEQ 0xadadadad

struct LogData
{
	LONG64 _idx;
	__int64 _timing;
	__int64 _headPtr;
	__int64 _headNext;
	__int64 _tailPtr;
	__int64 _tailNext;
};

extern __int64 g_logIndex;
extern LogData g_logArray[];

inline void Log(__int64 timing, __int64 headPtr, __int64 headNext, __int64 tailPtr, __int64 tailNext)
{
	LONG64 idx = InterlockedIncrement64(&g_logIndex);

	g_logArray[idx % LOG_ARRAY_LEN]._idx = idx;
	g_logArray[idx % LOG_ARRAY_LEN]._timing = timing;
	g_logArray[idx % LOG_ARRAY_LEN]._headPtr = headPtr;
	g_logArray[idx % LOG_ARRAY_LEN]._headNext = headNext;
	g_logArray[idx % LOG_ARRAY_LEN]._tailPtr = tailPtr;
	g_logArray[idx % LOG_ARRAY_LEN]._tailNext = tailNext;
}