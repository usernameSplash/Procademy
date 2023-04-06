#pragma once
class GameStageManager
{
private:
	GameStageManager();
	~GameStageManager();

public:
	static GameStageManager* GetInstance(void);
	void SetStageNum(int stageNum);
	int GetStageNum(void);

private:
	static GameStageManager sInstance;
	int mStageNum;
};

