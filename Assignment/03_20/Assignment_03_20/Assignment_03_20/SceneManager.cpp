
#include "TitleScene.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "OverScene.h"
#include "SceneManager.h"

#include "Renderer.h"
#include "DataFileNameLoader.h"

SceneManager SceneManager::sInstance;

SceneManager::SceneManager()
	: mCurScene(new TitleScene())
	, mNextScene(nullptr)
{

}

SceneManager::~SceneManager()
{
	delete mCurScene;
}

SceneManager* SceneManager::GetInstance(void)
{
	return &sInstance;
}

void SceneManager::SetNextScene(BaseScene* nextScene)
{
	mNextScene = nextScene;
}

bool SceneManager::Update(void)
{
	bool ret;

	Renderer::GetInstance()->BufferClear();
	ret = mCurScene->Update();
	mCurScene->Render();
	Renderer::GetInstance()->BufferFlip();

	if (mNextScene != nullptr)
	{
		delete mCurScene;

		mCurScene = mNextScene;
		mNextScene = nullptr;
	}

	return ret;
}
