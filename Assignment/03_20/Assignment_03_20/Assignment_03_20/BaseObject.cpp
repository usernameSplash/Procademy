#include "BaseObject.h"

BaseObject::BaseObject(eObjectType objectType, int x, int y)
	: mObjectType(objectType), mX(x), mY(y)
{
}

BaseObject::~BaseObject()
{
}

eObjectType BaseObject::GetObjectType(void)
{
	return mObjectType;
}