#include <Windows.h>
#include <process.h>

#include <cstdio>

void Lock0(void);
void Lock1(void);

void Unlock0(void);
void Unlock1(void);

bool flag[2];
int turn;

int wmain(void)
{
	return 0;
}

void Lock0(void)
{
	turn = 0;
	flag[0] = true;

	while (flag[1] && turn == 0)
	{

	}

	return;
}

void Lock1(void)
{
	turn = 1;
	flag[1] = true;
	
	while (flag[0] && turn == 1)
	{
	
	}

	return;
}

void Unlock0(void)
{
	flag[0] = false;
}

void Unlock1(void)
{
	flag[1] = false;
}
