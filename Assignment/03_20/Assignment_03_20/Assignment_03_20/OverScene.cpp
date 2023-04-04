#include "OverScene.h"
#include "SceneManager.h"

#include <conio.h>

bool OverScene::Update(void)
{
	if (_kbhit())
	{
		SceneManager::GetInstance()->SetScene(eSceneType::MENU);
	}
	return false;
}

void OverScene::Render(void)
{
	
}