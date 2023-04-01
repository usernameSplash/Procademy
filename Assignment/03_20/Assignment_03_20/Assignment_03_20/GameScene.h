#pragma once

#include "ConsoleManager.h"
#include "BaseScene.h"

class GameScene : public IBaseScene
{
protected:
	virtual void Update(void) override;
};

