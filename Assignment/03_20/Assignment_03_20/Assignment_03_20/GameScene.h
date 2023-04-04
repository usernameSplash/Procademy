#pragma once

#include "ConsoleManager.h"
#include "BaseScene.h"

class GameScene : public BaseScene
{
	virtual bool Update(void) override;
	virtual void Render(void) override;
};

