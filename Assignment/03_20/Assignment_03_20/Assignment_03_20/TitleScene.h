#pragma once

#include "BaseScene.h"
#include "Renderer.h"

class TitleScene : public BaseScene
{
public :
	TitleScene();

private:
	virtual bool Update(void) override;
	virtual void Render(void) override;

private:
	char mTitleBackground[dfSCREEN_HEIGHT * dfSCREEN_WIDTH];
	int mXPadding;
	int mYPadding;
};

