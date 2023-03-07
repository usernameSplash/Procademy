#pragma once
#include "BaseObject.h"

class ThreeStar : public BaseObject
{
public:
	ThreeStar();
	virtual bool Update(void) override;
	virtual void Render(void) override;
};