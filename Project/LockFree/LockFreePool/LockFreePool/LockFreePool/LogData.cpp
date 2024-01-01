#include "LogData.h"

__int64 g_pushCnt = -1;
__int64 g_popCnt = -1;

__int64 g_allocCnt = -1;
__int64 g_freeCnt = -1;

__int64 g_logIndex = -1;
LogData g_logArray[LOG_ARRAY_LEN];