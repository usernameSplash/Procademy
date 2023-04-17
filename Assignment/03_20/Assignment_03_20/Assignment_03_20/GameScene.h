#pragma once

#include "ConsoleManager.h"
#include "BaseScene.h"

class GameScene : public BaseScene
{
public:
	GameScene();

private:
	virtual bool Update(void) override;
	virtual void Render(void) override;
};
