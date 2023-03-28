#pragma once
#include "Console.h"

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
	char screenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
};

