#pragma once
#include <Windows.h>

class Clock
{
private:
	Clock();
	~Clock();

public:
	static Clock* GetInstance(void);
	void Tick(void);
	DWORD GetDeltaTime(void);
	void Reset(void);


private:
	static Clock sInstance;
	DWORD mPrevTime;
	DWORD mCurTime;
};

