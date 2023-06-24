#include "ConsoleManager.h"

void ConsoleManager::SetCursorPosition(const INT32 x, const INT32 y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };

	SetConsoleCursorPosition(handle, pos);
}

void ConsoleManager::SetCursorColor(const eConsoleColor color)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(handle, static_cast<WORD>(color));
}