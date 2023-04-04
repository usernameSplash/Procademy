#include <cstdio>
#include <cassert>
#include <cstring>
#include <conio.h>
#include "TitleScene.h"
#include "SceneManager.h"

TitleScene::TitleScene()
	: mYTimer(500, 0)
	, mIdleTimer(1000, 0)
	, mXPadding(3)
	, mYPadding(1)
	, mYPaddingAdditional(0)
	, mYDir(1)
{
	FILE* titleFile = fopen("./asciiarts/title.txt", "r");
	assert(titleFile != NULL);
	
	mYTimer.Activate();

	mObjectList.push_back(&mYTimer);
	mObjectList.push_back(&mIdleTimer);

	memset(mTitleBackground, ' ', sizeof(mTitleBackground));

	fread(mTitleBackground, sizeof(char), dfSCREEN_HEIGHT * dfSCREEN_WIDTH, titleFile);
}

bool TitleScene::Update(void)
{
	for (auto it = mObjectList.begin(); it != mObjectList.end(); ++it)
	{
		it->Update();
	}

	if (_kbhit())
	{
		SceneManager::GetInstance()->SetScene(eSceneType::MENU);
	}

	if (mYTimer.IsExpired())
	{
		mYPaddingAdditional += mYDir;
		
		if (mYPaddingAdditional % 2 == 0)
		{
			mYDir *= -1;
			mYTimer.Deactivate();
			mIdleTimer.Activate();
		}
	}

	if (mIdleTimer.IsExpired())
	{
		mYTimer.Activate();
		mIdleTimer.Deactivate();
	}

	return false;
}

void TitleScene::Render(void)
{
	Renderer* renderer = Renderer::GetInstance();

	const char* sentence = "Press Any Key";

	char* ch = &mTitleBackground[0];
	
	int x = mXPadding;
	int y = mYPadding + mYPaddingAdditional;

	while (*ch != '\0')
	{
		if (*ch == '\n')
		{
			y++;
			x = mXPadding;
		}
		else
		{
			renderer->SpriteDraw(x, y, *ch);
			x++;
		}

		ch++;
	}

	for (int x = 0; x < 13; x++)
	{
		renderer->SpriteDraw(x + 30, 22, sentence[x]);
	}
}