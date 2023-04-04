#pragma once
#include <Windows.h>

class Timer
{
private:
	Timer();
	~Timer();

public:
	static Timer* GetInstance(void);
	void Tick(void);
	DWORD GetDeltaTime(void);
	void Reset(void);


private:
	static Timer sInstance;
	DWORD mPrevTime;
	DWORD mCurTime;
};

