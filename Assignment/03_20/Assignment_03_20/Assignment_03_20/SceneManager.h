#pragma once

enum class eSceneType
{
	TITLE = 0,
	MENU,
	GAME,
	OVER
};

class IBaseScene;
class SceneManager
{
private:
	SceneManager();
	~SceneManager();

public:
	static SceneManager* GetInstance(void);
	void SetScene(eSceneType sceneType);
	void Update(void);

private:
	static SceneManager sInstance;
	IBaseScene* mCurScene;
	eSceneType mCurSceneType;
	bool mbSceneChanged;
};

