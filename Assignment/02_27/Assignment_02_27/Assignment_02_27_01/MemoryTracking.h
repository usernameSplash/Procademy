#pragma once


#ifdef MEMORY_TRACKING

void* operator new(size_t size, const char* fileName, size_t fileLine);
void* operator new[](size_t size, const char* fileName, size_t fileLine);
void operator delete(void* ptr, const char* fileName, size_t fileLine);
void operator delete[](void* ptr, const char* fileName, size_t fileLine);

void PrintMemoryLog(eMemoryErrorType error, AllocInfo* allocInfo, void* ptr, const char* fileName, size_t fileLine);

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
	int size;
	char filename[128];
	int line;
};

#endif // MEMORY_TRACKING

