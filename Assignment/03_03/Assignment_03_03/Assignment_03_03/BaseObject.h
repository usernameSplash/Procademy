#pragma once

class BaseObject
{
public:
	BaseObject() : x(0)
	{

	}

	virtual bool Update(void) = 0;
	virtual void Render(void) = 0;

protected:
	int x;
};