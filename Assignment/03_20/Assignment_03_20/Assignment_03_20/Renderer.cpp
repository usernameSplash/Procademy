#include <cstdio>
#include <cstring>
#include "Renderer.h"

Renderer::Renderer()
{
	BufferClear();
}

Renderer::~Renderer()
{
}

Renderer* Renderer::GetInstance(void)
{
	Renderer instance;
	return &instance;
}

void Renderer::BufferFlip(void)
{
	size_t row;
	for (row = 0; row < dfSCREEN_HEIGHT; row++)
	{
		cs_MoveCursor(0, row);
		printf(screenBuffer[row]);
	}
}

void Renderer::BufferClear(void)
{
	size_t row;
	memset(screenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (row = 0; row < dfSCREEN_HEIGHT; row++)
	{
		screenBuffer[row][dfSCREEN_WIDTH - 1] = '\0';
	}
}

void Renderer::SpriteDraw(int x, int y, char ch)
{
	if (x < 0 || y < 0 || x >= dfSCREEN_WIDTH - 1 || y >= dfSCREEN_HEIGHT)
	{
		return;
	}

	screenBuffer[y][x] = ch;
}