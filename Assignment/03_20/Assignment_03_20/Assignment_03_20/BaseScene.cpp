#include "BaseScene.h"

void BaseScene::Render(void)
{
	for (auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
	{
		it->Render();
	}
}