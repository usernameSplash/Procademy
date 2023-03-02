#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unordered_map>

#include "MemoryTracking.h" 

#define LOG_FILE_DIR "./log/"

static std::unordered_map<void*, AllocInfo> blocks(64);

void* operator new(size_t size, const char* fileName, size_t fileLine)
{
	void* ptr = malloc(size);

	AllocInfo allocInfo;

	allocInfo.ptr = ptr;
	allocInfo.size = size;
	strcpy(allocInfo.filename, fileName);
	allocInfo.line = fileLine;

	blocks.insert({ ptr, allocInfo });

	return ptr;
}

void* operator new[](size_t size, const char* fileName, size_t fileLine)
{
	void* ptr = malloc(size);

	AllocInfo allocInfo;

	allocInfo.ptr = ptr;
	allocInfo.size = size;
	strcpy(allocInfo.filename, fileName);
	allocInfo.line = fileLine;

	blocks.insert({ ptr, allocInfo });

	return ptr;
}

void operator delete(void* ptr, const char* fileName, size_t fileLine)
{
	eMemoryErrorType e;

	if (blocks.find(ptr) == blocks.end())
	{
		auto result = blocks.find((void*)(__int64(ptr) - sizeof(size_t)));
		if (result == blocks.end())
		{
			e = eMemoryErrorType::NOALLOC;

			PrintMemoryLog(e, nullptr, ptr, fileName, fileLine);
		}
		else
		{
			e = eMemoryErrorType::ARRAY;
			PrintMemoryLog(e, &blocks.at(ptr), ptr, fileName, fileLine);
		}

	}
	else
	{
		blocks.erase(ptr);
		free(ptr);
	}
}

void operator delete[](void* ptr, const char* fileName, size_t fileLine)
{
	eMemoryErrorType e;

	if (blocks.find(ptr) == blocks.end())
	{
		auto result = blocks.find((void*)(__int64(ptr) + sizeof(size_t)));
		if (result == blocks.end())
		{
			e = eMemoryErrorType::NOALLOC;

			PrintMemoryLog(e, nullptr, ptr, fileName, fileLine);
		}
		else
		{
			e = eMemoryErrorType::ARRAY;
			PrintMemoryLog(e, &blocks.at(ptr), ptr, fileName, fileLine);
		}

	}
	else
	{
		blocks.erase(ptr);
		free(ptr);
	}
}

void PrintMemoryLog(eMemoryErrorType error, AllocInfo* allocInfo, void* ptr, const char* fileName, size_t fileLine)
{
	static FILE* logFile = nullptr;
	static char logFileName[50] = "";

	if (logFile == nullptr)
	{
		time_t timeStamp;
		struct tm localTime;

		timeStamp = time(NULL);
		localtime_s(&localTime, &timeStamp);

		snprintf(logFileName, 50, LOG_FILE_DIR"MemoryTracking_%04d%2d%2d_%2d%2d%2d.txt", localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
			localTime.tm_hour + 1, localTime.tm_min, localTime.tm_sec);
	}

	logFile = fopen(logFileName, "a");

	switch (error)
	{
	case eMemoryErrorType::NOALLOC:
		fscanf(logFile, "%-8s [%p]", "NOALLOC", ptr);
		break;
	case eMemoryErrorType::ARRAY:
		fscanf(logFile, "%-8s [%p] [%8d] %s : %d", "ARRAY", allocInfo->ptr, allocInfo->size, fileName, fileLine);
		break;
	case eMemoryErrorType::LEAK:
		fscanf(logFile, "%-8s [%p] [%8d] %s : %d", "LEAK", allocInfo->ptr, allocInfo->size, fileName, fileLine);
		break;
	}

	fputc('\n', logFile);

	fclose(logFile);
}


#define new new(__FILE__, __LINE__)
// #define delete ... // there is no delete define because we can't call overloaded delete explicitly.