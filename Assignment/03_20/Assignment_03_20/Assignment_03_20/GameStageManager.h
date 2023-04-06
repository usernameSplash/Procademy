#pragma once

class GameStageManager
{
private:
	GameStageManager();
	~GameStageManager();

public:
	static GameStageManager* GetInstance(void);
	void SetStageNum(int stageNum);
	int GetCurStageNum(void);

private:
	static GameStageManager sInstance;
	int* mStageHighestScore;
	int mCurStageNum;
};

