#include "MenuScene.h"
#include "DataFileNameLoader.h"
#include "Renderer.h"
#include "List.h"
#include "SceneManager.h"

#include <Windows.h>
#include <conio.h>

MenuScene::MenuScene()
	: mSelectIndex(0)
	, mStageCount(DataFileNameLoader::GetInstance()->GetStageCount())
	, mXPadding(35)
	, mYPadding(5)
{

}

bool MenuScene::Update(void)
{
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		if (mSelectIndex > 0)
		{
			mSelectIndex--;
		}
	}

	else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		if (mSelectIndex < mStageCount - 1)
		{
			mSelectIndex++;
		}
	}

	else if (GetAsyncKeyState(VK_RETURN) & 0x8001)
	{
		SceneManager::GetInstance()->SetScene(eSceneType::GAME);
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