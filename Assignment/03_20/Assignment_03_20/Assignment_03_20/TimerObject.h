#pragma once

#include "BaseObject.h"
#include "Clock.h"

class TimerObject : public BaseObject
{
private:
	virtual bool Update(void) override;
	virtual void Render(void) override;

public:
	TimerObject(DWORD interval, DWORD time);
	void Activate();
	void Deactivate();
	void SetInterval(DWORD interval);
	void SetTime(DWORD time);
	bool IsExpired(void);

private:
	bool mbActivated;
	DWORD mInterval;
	DWORD mTime;
};

