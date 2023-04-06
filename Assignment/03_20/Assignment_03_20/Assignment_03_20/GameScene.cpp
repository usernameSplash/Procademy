#include "GameScene.h"

#include "DataFileNameLoader.h"
#include "GameStageManager.h"

#include <cstdio>
#include <cstring>

GameScene::GameScene()
{
	FILE* file;
	char file_path[1024] = STAGE_INFO_FILE_DIR;

	strcat(file_path, DataFileNameLoader::GetInstance()->GetStageFileName(GameStageManager::GetInstance()->GetCurStageNum()));
	file = fopen(file_path, "r");
}

bool GameScene::Update(void)
{
	return false;
}

void GameScene::Render(void)
{

}