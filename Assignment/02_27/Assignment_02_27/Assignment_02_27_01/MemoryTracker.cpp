#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unordered_map>

#include "MemoryTracker.h" 

#undef new

static MemoryTracker s_MemoryTracker;

MemoryTracker::MemoryTracker(const char* outputFileName)
{
	char timeString[20];
	time_t timer;
	struct tm TM;

	memset(mAllocInfos, 0, sizeof(ALLOC_INFO) * ALLOC_INFO_INITIAL_SIZE);
	memset(mOutputFileName, 0, FILE_NAME_LEN);

	time(&timer);
	localtime_s(&TM, &timer);

	sprintf(timeString, "%04d%02d%02d_%02d%02d%02d",
		TM.tm_year + 1900,
		TM.tm_mon + 1,
		TM.tm_mday,
		TM.tm_hour,
		TM.tm_min,
		TM.tm_sec);
	
	strncat(mOutputFileName, outputFileName, FILE_NAME_LEN - 17 - 4);
	strncat(mOutputFileName, timeString, 20);
	strncat(mOutputFileName, ".txt", 4);
}

MemoryTracker::~MemoryTracker()
{
	SaveLogFile();
}

bool MemoryTracker::SaveLogFile(void)
{
	FILE* logFile;
	errno_t err;

	err = fopen_s(&logFile, mOutputFileName, "a");

	if (err != 0)
	{
		return false;
	}

	for (size_t i = 0; i < ALLOC_INFO_INITIAL_SIZE; i++)
	{
		if (mAllocInfos[i].pAddr != nullptr)
		{
			fprintf(logFile, "%-7s ", "LEAK");
			fprintf(logFile, "[0x%p] [%7zu] %s : %zu\n",
				mAllocInfos[i].pAddr,
				mAllocInfos[i].size,
				mAllocInfos[i].fileName,
				mAllocInfos[i].fileLine);
		}
	}

	fclose(logFile);

	return true;
}

bool MemoryTracker::New(void* ptr, const char* fileName, size_t fileLine, size_t size, bool bArray)
{
	for (size_t i = 0; i < ALLOC_INFO_INITIAL_SIZE; i++)
	{
		if (mAllocInfos[i].pAddr == nullptr)
		{
			mAllocInfos[i].pAddr = ptr;
			mAllocInfos[i].size = size;
			strncpy(mAllocInfos[i].fileName, fileName, FILE_NAME_LEN);
			mAllocInfos[i].fileLine = fileLine;
			mAllocInfos[i].bArray = bArray;

			return true;
		}
	}

	return false;
}

bool MemoryTracker::Delete(void* ptr, bool bArray)
{
	FILE* logFile;
	errno_t err;
	
	for (size_t i = 0; i < ALLOC_INFO_INITIAL_SIZE; i++)
	{
		if (mAllocInfos[i].pAddr == ptr)
		{
			if (mAllocInfos[i].bArray != bArray)
			{
				err = fopen_s(&logFile, mOutputFileName, "a");

				if (err == 0)
				{
					fprintf(logFile, "%-7s ", "ARRAY");
					fprintf(logFile, "[0x%p] [%7zu] %s : %zu\n",
						mAllocInfos[i].pAddr,
						mAllocInfos[i].size,
						mAllocInfos[i].fileName,
						mAllocInfos[i].fileLine);
					fclose(logFile);
				}
				return false;
			}

			mAllocInfos[i].pAddr = nullptr;
			return true;
		}
	}

	err = fopen_s(&logFile, mOutputFileName, "a");

	if (err == 0)
	{
		fprintf(logFile, "%-7s ", "NOALLOC");
		fprintf(logFile, "[0x%p]\n", ptr);
		fclose(logFile);
	}
	return false;
}

void* operator new(size_t size, const char* fileName, size_t line)
{
	void* ptr = malloc(size);
	s_MemoryTracker.New(ptr, fileName, line, size, false);
	return ptr;
}

void* operator new[](size_t size, const char* fileName, size_t line)
{
	void* ptr = malloc(size);
	s_MemoryTracker.New(ptr, fileName, line, size, true);
	return ptr;
}

void operator delete(void* ptr)
{
	if (s_MemoryTracker.Delete(ptr, false))
	{
		free(ptr);
	}
}

void operator delete[](void* ptr)
{
	if (s_MemoryTracker.Delete(ptr, true))
	{
		free(ptr);
	}
}

void operator delete(void* ptr, const char* fileName, size_t line)
{
}

void operator delete[](void* ptr, const char* fileName, size_t line) 
{
}

#define new new(__FILE__, __LINE__)
// #define delete ... // there is no delete define because we can't call overloaded delete explicitly.
