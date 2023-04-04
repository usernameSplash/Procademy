#pragma once

#include "BaseScene.h"

class MenuScene : public BaseScene
{
public :
	MenuScene();

private:
	virtual bool Update(void) override;
	virtual void Render(void) override;

public:
	 
};

