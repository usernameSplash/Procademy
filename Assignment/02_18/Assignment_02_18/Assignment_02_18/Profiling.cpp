#include "Profiling.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <cstdlib>

#define DEFAULT_PROFILE_NUM 32

static std::unordered_map<std::string, Profile_t> s_profileMap;

static LARGE_INTEGER s_frequency;

void ProfileBegin(const std::string& tagName)
{
	if (s_profileMap.find(tagName) == s_profileMap.end())
	{
		Profile_t profile;
		memset(&profile, 0, sizeof(profile));
		s_profileMap.insert({ tagName, profile });
	}

	if (s_profileMap[tagName].flag)
	{
		return;
	}

	s_profileMap[tagName].flag = true;
	QueryPerformanceCounter(&s_profileMap[tagName].startTime);

	s_profileMap[tagName].callTime++;
}

void ProfileEnd(const std::string& tagName)
{
	LARGE_INTEGER endTime;
	LARGE_INTEGER resultTime;

	if (s_profileMap.find(tagName) == s_profileMap.end())
	{
		return;
	}
	
	Profile_t& profile = s_profileMap[tagName];

	if (profile.flag != true)
	{
		return;
	}

	profile.flag = false;
	QueryPerformanceCounter(&endTime);

	resultTime.QuadPart = endTime.QuadPart - profile.startTime.QuadPart;
	profile.totalTime += resultTime.QuadPart;

	if ((UINT64)resultTime.QuadPart > profile.maxTime[0])
	{
		profile.maxTime[1] = profile.maxTime[0];
		profile.maxTime[0] = resultTime.QuadPart;
	}
	else if ((UINT64)resultTime.QuadPart > profile.maxTime[1])
	{
		profile.maxTime[1] = resultTime.QuadPart;
	}
	else if ((UINT64)resultTime.QuadPart < profile.minTime[0])
	{
		profile.minTime[1] = profile.minTime[0];
		profile.minTime[0] = resultTime.QuadPart;
	}
	else if ((UINT64)resultTime.QuadPart < profile.minTime[1])
	{
		profile.minTime[1] = resultTime.QuadPart;
	}
}

void ProfileDataOutText(const char* fileName)
{
	std::ofstream file(fileName, std::ios::binary);

	file << std::setw(132) << std::setfill('-') << "\n";

	file << std::setw(DEFAULT_PROFILE_NUM) << std::setfill(' ') << "Name" << "  |" << std::setw(20) << "Average" << "  |" << std::setw(20) << "Min" << "  |" << std::setw(20) << "Max" << "  |" <<
		std::setw(20) << "Call" << std::endl;

	for (std::unordered_map<std::string, Profile_t>::const_iterator it = s_profileMap.begin(); it != s_profileMap.end(); it++)
	{
		UINT64 avg;
		double avgTime;
		double minTime;
		double maxTime;

		avg = it->second.totalTime - (it->second.maxTime[0] + it->second.maxTime[1] + it->second.minTime[0] + it->second.minTime[1]);
		avgTime = ((double)avg / it->second.callTime) / s_frequency.QuadPart;
		minTime = (double)it->second.minTime[0] / s_frequency.QuadPart;
		maxTime = (double)it->second.maxTime[0] / s_frequency.QuadPart;

		file << std::setw(DEFAULT_PROFILE_NUM) << std::setfill(' ') << it->first << "  |" << std::setw(18) << avgTime * 1000000 << "§Á  |" << std::setw(18) << minTime * 1000000 << "§Á  |" << std::setw(18) << maxTime * 1000000 << "§Á  |" <<
			std::setw(20) << it->second.callTime << std::endl;
	}

	file << std::setw(132) << std::setfill('-') << "\n";
	file << std::endl;
}

void ProfileReset(void)
{
	QueryPerformanceFrequency(&s_frequency);
	s_profileMap.clear();
	s_profileMap.reserve(DEFAULT_PROFILE_NUM);
}