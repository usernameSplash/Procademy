#include "MenuScene.h"

#include "DataFileNameLoader.h"
#include "Renderer.h"
#include "List.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "GameStageManager.h"

#include <Windows.h>
#include <conio.h>

MenuScene::MenuScene()
	: mSelectIndex(0)
	, mStageCount(DataFileNameLoader::GetInstance()->GetStageCount())
	, mXPadding(35)
	, mYPadding(5)
	, mbUpPressed(false)
	, mbDownPressed(false)
{

}

bool MenuScene::Update(void)
{
	if (GetAsyncKeyState(VK_UP) & 0x8001)
	{
		if (mbUpPressed == false)
		{
			mbUpPressed = true;
			if (mSelectIndex > 0)
			{
				mSelectIndex--;
			}
		}
	}
	else
	{
		mbUpPressed = false;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		if (mbDownPressed == false)
		{
			mbDownPressed = true;
			if (mSelectIndex < mStageCount - 1)
			{
				mSelectIndex++;
			}
		}
	}
	else
	{
		mbDownPressed = false;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8001)
	{
		GameStageManager::GetInstance()->SetStageNum(mSelectIndex);
		SceneManager::GetInstance()->SetNextScene(new GameScene());
	}

	return false;
}

void MenuScene::Render(void)
{
	MyDataStructure::List<int> l;
	const char* str = "Stage";

	for (int iCnt = 0; iCnt < mStageCount; iCnt++)
	{
		int stage = iCnt + 1;

		for (int jCnt = 0; jCnt < 5; jCnt++)
		{
			Renderer::GetInstance()->SpriteDraw(mXPadding + jCnt, mYPadding + iCnt, str[jCnt]);
		}

		while (stage != 0)
		{
			l.push_front(stage % 10);
			stage /= 10;
		}

		for (int jCnt = 0; jCnt < l.size(); jCnt++)
		{
			Renderer::GetInstance()->SpriteDraw(mXPadding + jCnt + 5, mYPadding + iCnt, (l.pop_front() + '0'));
		}
	}

	Renderer::GetInstance()->SpriteDraw(mXPadding - 3, mYPadding + mSelectIndex, '>');
}