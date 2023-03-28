#include "ConsoleManager.h"

ConsoleManager::ConsoleManager()
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
}

ConsoleManager::~ConsoleManager()
{
}

ConsoleManager* ConsoleManager::GetInstance(void)
{
	static ConsoleManager instance;
	return &instance;
}

void ConsoleManager::MoveCursor(int x, int y)
{
	COORD targetCoord;
	targetCoord.X = x;
	targetCoord.Y = y;

	SetConsoleCursorPosition(hConsole, targetCoord);
}

void ConsoleManager::ClearScreen(void)
{
	DWORD dw;
	FillConsoleOutputCharacter(hConsole, ' ', 100 * 100, { 0, 0 }, &dw);
}