#include "GameStageManager.h"
#include "DataFileNameLoader.h"

GameStageManager GameStageManager::sInstance;

GameStageManager::GameStageManager()
	: mStageHighestScore(new int[DataFileNameLoader::GetInstance()->GetStageCount()])
	, mCurStageNum(0)
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
		mCurStageNum = stageNum;
	}
}

int GameStageManager::GetCurStageNum(void)
{
	return mCurStageNum;
}