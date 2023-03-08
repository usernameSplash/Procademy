#pragma once

#include <unordered_map>

void* operator new(size_t size, const char* fileName, size_t fileLine);
void* operator new[](size_t size, const char* fileName, size_t fileLine);
void operator delete(void* ptr);
void operator delete[](void* ptr);

class MemoryTracker
{
public:
	enum
	{
		FILE_NAME_LEN = 128,
		ALLOC_INFO_INITIAL_SIZE = 1024
	};

	struct ALLOC_INFO
	{
		void* pAddr;
		size_t size;
		char fileName[FILE_NAME_LEN];
		size_t fileLine;
		bool bArray;
	};

private:
	ALLOC_INFO mAllocInfos[ALLOC_INFO_INITIAL_SIZE];
	char mOutputFileName[FILE_NAME_LEN];

public:
	MemoryTracker(const char* outputFileName = "AllocInfo");
	~MemoryTracker();

private:
	bool SaveLogFile(void);
	bool New(void* ptr, const char* fileName, size_t fileLine, size_t size, bool bArray);
	bool Delete(void* ptr, bool bArray);

	friend void* operator new(size_t size, const char* fileName, size_t fileLine);
	friend void* operator new[](size_t size, const char* fileName, size_t fileLine);
	friend void operator delete(void* ptr);
	friend void operator delete[](void* ptr);
};

#define new new(__FILE__, __LINE__)
// #define delete ... // there is no delete define because we can't call overloaded delete explicitly.
