#include <cstdio>
#include <cassert>
#include <cstring>
#include "TitleScene.h"

TitleScene::TitleScene()
	: mXPadding(0)
	, mYPadding(0)
{
	FILE* titleFile = fopen("./asciiarts/title.txt", "r");
	assert(titleFile != NULL);

	memset(mTitleBackground, ' ', sizeof(mTitleBackground));

	fread(mTitleBackground, sizeof(char), dfSCREEN_HEIGHT * dfSCREEN_WIDTH, titleFile);
}

bool TitleScene::Update(void)
{
	mXPadding++;
	mXPadding %= 3;

	mYPadding++;
	mYPadding %= 3;

	return false;
}

void TitleScene::Render(void)
{
	Renderer* renderer = Renderer::GetInstance();

	const char* sentence = "Press Any Key";

	char* ch = &mTitleBackground[0];
	
	int x = mXPadding;
	int y = mYPadding;

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