#pragma once

class IBaseScene
{
protected:
	virtual void Update() = 0;

	friend class SceneManager;
};

