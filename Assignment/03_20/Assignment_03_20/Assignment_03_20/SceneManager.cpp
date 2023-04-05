
#include "TitleScene.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "OverScene.h"
#include "SceneManager.h"

#include "Renderer.h"

SceneManager SceneManager::sInstance;

SceneManager::SceneManager()
	: mCurScene(new TitleScene())
	, mCurSceneType(eSceneType::TITLE)
	, mbSceneChanged(false)
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

void SceneManager::SetScene(eSceneType sceneType)
{
	if (mCurSceneType == sceneType)
	{
		return;
	}

	mCurSceneType = sceneType;
	mbSceneChanged = true;
}

bool SceneManager::Update(void)
{
	bool ret;

	Renderer::GetInstance()->BufferClear();
	ret = mCurScene->Update();
	mCurScene->Render();
	Renderer::GetInstance()->BufferFlip();

	if (mbSceneChanged)
	{
		delete mCurScene;
		mbSceneChanged = false;

		switch (mCurSceneType)
		{
		case eSceneType::TITLE:
			mCurScene = new TitleScene();
			break;
		case eSceneType::MENU:
			mCurScene = new MenuScene();
			break;
		case eSceneType::GAME:
			mCurScene = new GameScene();
			break;
		case eSceneType::OVER:
			mCurScene = new OverScene();
			break;
		default:
			break;
		}
	}

	return ret;
}