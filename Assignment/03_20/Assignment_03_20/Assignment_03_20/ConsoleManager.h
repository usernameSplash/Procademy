#pragma once
#include <Windows.h>

#define dfSCREEN_WIDTH		81		// �ܼ� ���� 80ĭ + NULL
#define dfSCREEN_HEIGHT		24		// �ܼ� ���� 24ĭ

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

