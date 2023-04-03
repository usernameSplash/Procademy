#include "PlayerObject.h"

PlayerObject::PlayerObject(eObjectType objectType, int x, int y)
	: BaseObject(objectType, x, y, 'A')
{

}

bool PlayerObject::Update(void)
{
	return false;
}

void PlayerObject::Render(void)
{
	
}