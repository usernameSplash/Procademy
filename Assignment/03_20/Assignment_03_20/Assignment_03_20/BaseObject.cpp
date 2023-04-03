#include "BaseObject.h"

BaseObject::BaseObject(eObjectType objectType, int x, int y, char sprite)
	: mObjectType(objectType)
	, mX(x)
	, mY(y)
	, mSprite(sprite)
{
}

BaseObject::~BaseObject()
{
}

eObjectType BaseObject::GetObjectType(void)
{
	return mObjectType;
}