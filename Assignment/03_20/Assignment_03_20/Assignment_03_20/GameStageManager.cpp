#include "GameStageManager.h"
#include "DataFileNameLoader.h"

GameStageManager GameStageManager::sInstance;

GameStageManager::GameStageManager()
	: mStageNum(0)
{

}

GameStageManager::~GameStageManager()
{

}

GameStageManager* GameStageManager::GetInstance(void)
{
	return &sInstance;
}

void GameStageManager::SetStageNum(int stageNum)
{
	if (0 <= stageNum && stageNum < DataFileNameLoader::GetInstance()->GetStageCount())
	{
		mStageNum = stageNum;
	}
}

int GameStageManager::GetStageNum(void)
{
	return mStageNum;
}