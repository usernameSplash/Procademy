#include "OverScene.h"
#include "MenuScene.h"
#include "SceneManager.h"

#include <conio.h>

bool OverScene::Update(void)
{
	if (_kbhit())
	{
		SceneManager::GetInstance()->SetNextScene(new MenuScene());
	}
	return false;
}

void OverScene::Render(void)
{
	
}