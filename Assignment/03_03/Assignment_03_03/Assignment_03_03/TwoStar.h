#pragma once
#include "BaseObject.h"

class TwoStar : public BaseObject
{
public:
	TwoStar();
	virtual bool Update(void) override;
	virtual void Render(void) override;
};