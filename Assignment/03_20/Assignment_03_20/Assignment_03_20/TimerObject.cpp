#include "TimerObject.h"

TimerObject::TimerObject(DWORD interval, DWORD time)
	: BaseObject(eObjectType::Invisible, 0, 0, ' ')
	, mbActivated(false)
	, mInterval(interval)
	, mTime(time)
{

}

bool TimerObject::Update(void)
{
	if (mbActivated == false)
	{
		return false;
	}

	if (mTime >= mInterval)
	{
		return false;
	}

	DWORD deltaTime;
	
	deltaTime = Clock::GetInstance()->GetDeltaTime();
	mTime += deltaTime;
	

	return false;
}

void TimerObject::Render(void)
{
	// Do Nothing
	return;
}

void TimerObject::Activate(void)
{
	mbActivated = true;
}

void TimerObject::Deactivate(void)
{
	mbActivated = false;
}

void TimerObject::SetInterval(DWORD interval)
{
	mInterval = interval;
}

void TimerObject::SetTime(DWORD time)
{
	mTime = time;
}

bool TimerObject::IsExpired(void)
{
	if (mTime >= mInterval)
	{
		mTime = 0;
		return true;
	}

	return false;
}
