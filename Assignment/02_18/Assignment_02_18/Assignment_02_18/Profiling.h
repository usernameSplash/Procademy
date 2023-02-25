#pragma once

#include <Windows.h>
#include <string>

#ifdef PROFILE
#define PRO_BEGIN(TagName) ProfileBegin(TagName)
#define PRO_END(TagName) ProfileEnd(TagName)
#else
#define PRO_BEGIN(TagName)
#define PRO_END(TagName)
#endif // PROFILE

typedef struct Profile
{
	bool flag;
	LARGE_INTEGER startTime;

	UINT64 totalTime;
	UINT64 minTime[2];
	UINT64 maxTime[2];
	UINT64 callTime;
} Profile_t;

// Begin profiling for a job having <tagName>
void ProfileBegin(const std::string& tagName);

// End profiling for a job having <tagName>
void ProfileEnd(const std::string& tagName);

// Print profile data by text file
void ProfileDataOutText(const char* fileName);

// Reset All Profile Data
void ProfileReset(void);