#include "Renderer.h"

#include <cstdio>
#include "ConsoleManager.h"

wchar_t g_screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

void BufferFlip(void)
{
	int row;
	for (row = 0; row < SCREEN_HEIGHT; row++)
	{
		ConsoleManager::GetInstance()->MoveCursor(0, row);
		wprintf(g_screenBuffer[row]);
	}
}

void BufferClear(void)
{
	size_t row;
	size_t column;

	for (row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (column = 0; column < SCREEN_WIDTH; column++)
		{
			g_screenBuffer[row][column] = L' ';
		}
	}

	for (row = 0; row < SCREEN_HEIGHT; row++)
	{
		g_screenBuffer[row][SCREEN_WIDTH - 1] = L'\0';
	}
}

void SpriteDraw(int x, int y, wchar_t ch)
{
	if (x < 0 || y < 0 || x >= SCREEN_WIDTH - 1 || y >= SCREEN_HEIGHT)
	{
		return;
	}

	g_screenBuffer[y][x] = ch;
}