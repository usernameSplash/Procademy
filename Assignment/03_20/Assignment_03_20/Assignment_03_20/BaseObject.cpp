#include "BaseObject.h"

BaseObject::BaseObject(eObjectType objectType, int x, int y, char sprite = ' ')
	: mObjectType(objectType)
	, mX(x)
	, mY(y)
	, mSprite(sprite)
{
}

BaseObject::~BaseObject()
{
}

void BaseObject::SetPos(int x, int y)
{
	mX = x;
	mY = y;
}

eObjectType BaseObject::GetObjectType(void)
{
	return mObjectType;
}