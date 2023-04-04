#include "Clock.h"

#include <timeapi.h>
#pragma comment(lib, "Winmm.lib") // for using timeGetTime function

Clock Clock::sInstance;

Clock::Clock()
	: mPrevTime(0)
	, mCurTime(timeGetTime())
{
	timeBeginPeriod(1);
}

Clock::~Clock()
{

}

Clock* Clock::GetInstance(void)
{
	return &sInstance;
}

void Clock::Tick(void)
{
	mPrevTime = mCurTime;
	mCurTime = timeGetTime();
}

DWORD Clock::GetDeltaTime(void)
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

void Clock::Reset(void)
{
	mPrevTime = 0;
	mCurTime = 0;
}