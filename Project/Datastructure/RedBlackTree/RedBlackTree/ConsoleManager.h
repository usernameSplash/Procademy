#pragma once

#include <Windows.h>

class ConsoleManager
{
public:
	enum class eConsoleColor
	{
		BLACK = 0,
		RED = FOREGROUND_RED,
		GREEN = FOREGROUND_GREEN,
		BLUE = FOREGROUND_BLUE,
		YELLOW = RED | GREEN,
		WHITE = RED | GREEN | BLUE,
	};
public:
	static void SetCursorPosition(const INT32 x, const INT32 y);
	static void SetCursorColor(const eConsoleColor color);
	static void ShowConsoleColor(const bool flag);
};