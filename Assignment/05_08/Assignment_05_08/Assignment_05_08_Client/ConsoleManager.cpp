#include "ConsoleManager.h"

ConsoleManager ConsoleManager::sInstance;

ConsoleManager::ConsoleManager()
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	mhConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(mhConsole, &stConsoleCursor);
}

ConsoleManager::~ConsoleManager()
{
}

ConsoleManager* ConsoleManager::GetInstance(void)
{
	return &sInstance;
}

void ConsoleManager::MoveCursor(int x, int y)
{
	COORD targetCoord;
	targetCoord.X = x;
	targetCoord.Y = y;

	SetConsoleCursorPosition(mhConsole, targetCoord);
}

void ConsoleManager::ClearScreen(void)
{
	DWORD dw;
	FillConsoleOutputCharacter(mhConsole, ' ', 100 * 100, { 0, 0 }, &dw);
}