#pragma once
#include <Windows.h>

#define dfSCREEN_WIDTH		81		// 콘솔 가로 80칸 + NULL
#define dfSCREEN_HEIGHT		24		// 콘솔 세로 24칸

class ConsoleManager
{
private:
	ConsoleManager();
	~ConsoleManager();

public :
	static ConsoleManager* GetInstance(void);
	void MoveCursor(int x, int y);
	void ClearScreen(void);

private:
	HANDLE hConsole;
};

