#pragma once

#include <basetsd.h>

#define SCREEN_WIDTH 82
#define SCREEN_HEIGHT 24

extern wchar_t g_screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

void BufferFlip(void);
void BufferClear(void);
void SpriteDraw(int x, int y, wchar_t ch);