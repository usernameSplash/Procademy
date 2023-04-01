#pragma once
#include "ConsoleManager.h"

class Renderer
{
private:
	Renderer();
	~Renderer();

public:
	static Renderer* GetInstance(void);

	void BufferFlip(void);
	void BufferClear(void);
	void SpriteDraw(int x, int y, char ch);

private:
	static Renderer sInstance;
	char screenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
};