#pragma once

#include "BaseObject.h"

class BulletObject : public BaseObject
{
	virtual bool Update(void);
	virtual void Render(void);
};

