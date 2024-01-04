#include "LogData.h"

__int64 g_enqCnt = 0;
__int64 g_deqCnt = 0;

__int64 g_logIndex = -1;
LogData g_logArray[LOG_ARRAY_LEN];

__int64 g_casFailLogIndex = -1;
LogData g_casFailLogArray[FAIL_LOG_ARRAY_LEN];