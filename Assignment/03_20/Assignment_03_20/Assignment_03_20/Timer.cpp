#include "Timer.h"

#include <timeapi.h>
#pragma comment(lib, "Winmm.lib") // for using timeGetTime function

Timer Timer::sInstance;

Timer::Timer()
	: mPrevTime(0)
	, mCurTime(0)
{
	timeBeginPeriod(1);
}

Timer::~Timer()
{

}

Timer* Timer::GetInstance(void)
{
	return &sInstance;
}

void Timer::Tick(void)
{
	mPrevTime = mCurTime;
	mCurTime = timeGetTime();
}

DWORD Timer::GetDeltaTime(void)
{
	DWORD deltaTime;

	if (mCurTime < mPrevTime)
	{
		deltaTime = MAXDWORD - mPrevTime + mCurTime;
	}
	else
	{
		deltaTime = mCurTime - mPrevTime;
	}

	return deltaTime;
}

void Timer::Reset(void)
{
	mPrevTime = 0;
	mCurTime = 0;
}