#pragma once


#ifdef MEMORY_TRACKING

void* operator new(size_t size, const char* fileName, int fileLine);
void* operator new[](size_t size, const char* fileName, int fileLine);
void operator delete(void* ptr, const char* fileName, int fileLine);
void operator delete[](void* ptr, const char* fileName, int fileLine);

enum class eMemoryErrorType
{
	NOERROR = 0,
	NOALLOC,
	ARRAY,
	LEAK
};

struct AllocInfo
{
	void* ptr;
	size_t size;
	char filename[128];
	int line;
};

void PrintMemoryLog(eMemoryErrorType error, AllocInfo* allocInfo, void* ptr, const char* fileName, int fileLine);

#endif // MEMORY_TRACKING

