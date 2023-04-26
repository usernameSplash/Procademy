#pragma once
#include <Windows.h>

class ConsoleManager
{
private:
	ConsoleManager();
	~ConsoleManager();

public:
	static ConsoleManager* GetInstance(void);
	void MoveCursor(int x, int y);
	void ClearScreen(void);

private:
	static ConsoleManager sInstance;
	HANDLE mhConsole;
};
