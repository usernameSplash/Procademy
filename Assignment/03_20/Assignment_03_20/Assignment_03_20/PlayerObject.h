#pragma once

#include "BaseObject.h"

class PlayerObject : public BaseObject
{
public:
	PlayerObject(eObjectType objectType, int x, int y);

private:
	virtual bool Update(void);
	virtual void Render(void);
};

