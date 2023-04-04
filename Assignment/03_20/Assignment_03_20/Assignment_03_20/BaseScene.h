#pragma once

#include "List.h"
#include "BaseObject.h"

class BaseScene
{
protected:
	virtual bool Update(void) = 0;
	virtual void Render(void) = 0;

protected:
	MyDataStructure::List<BaseObject*> mObjectList;

	friend class SceneManager;
};

