#pragma once

enum class eSceneType
{
	TITLE = 0,
	MENU,
	GAME,
	OVER
};

class BaseScene;
class SceneManager
{
private:
	SceneManager();
	~SceneManager();

public:
	static SceneManager* GetInstance(void);
	void SetScene(eSceneType sceneType);
	bool Update(void);
	void Render(void);

private:
	static SceneManager sInstance;
	BaseScene* mCurScene;
	eSceneType mCurSceneType;
	bool mbSceneChanged;
};

