#pragma once

#include "BaseObject.h"

class EnemyObject : public BaseObject
{
	virtual bool Update(void);
	virtual void Render(void);
};

