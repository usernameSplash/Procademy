#pragma once
#include "BaseObject.h"

class OneStar : public BaseObject
{
public :
	OneStar();
	virtual bool Update(void) override;
	virtual void Render(void) override;
};