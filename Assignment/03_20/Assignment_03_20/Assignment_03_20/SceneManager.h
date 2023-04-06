#pragma once

class BaseScene;
class SceneManager
{
private:
	SceneManager();
	~SceneManager();

public:
	static SceneManager* GetInstance(void);
	void SetNextScene(BaseScene* nextScene);
	bool Update(void);

private:
	static SceneManager sInstance;
	BaseScene* mCurScene;
	BaseScene* mNextScene;
};

