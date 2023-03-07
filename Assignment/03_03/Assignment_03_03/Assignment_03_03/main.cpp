#include <Windows.h>
#include <conio.h>
#include <cstdio>
#include "BaseObject.h"
#include "OneStar.h"
#include "TwoStar.h"
#include "ThreeStar.h"

#define OBJECT_NUM 20

static BaseObject* s_objectPool[OBJECT_NUM] = { 0, };

void Action(void)
{
	for (unsigned int objectIdx = 0; objectIdx < OBJECT_NUM; objectIdx++)
	{
		bool result;

		if (s_objectPool[objectIdx] == nullptr)
		{
			continue;
		}

		result = s_objectPool[objectIdx]->Update();

		if (!result)
		{
			delete s_objectPool[objectIdx];
			s_objectPool[objectIdx] = nullptr;
		}
	}
}

void Draw(void)
{
	for (unsigned int objectIdx = 0; objectIdx < OBJECT_NUM; objectIdx++)
	{

		if (s_objectPool[objectIdx] == nullptr)
		{
			printf("\n");
			continue;
		}

		s_objectPool[objectIdx]->Render();
	}
}

void InputProcess(void)
{
	int key;
	BaseObject* obj = nullptr;

	if (_kbhit())
	{
		key = _getch();

		switch (key)
		{
		case '1':
			obj = new OneStar;
			break;
			
		case '2':
			obj = new TwoStar;
			break;

		case '3':
			obj = new ThreeStar;
			break;
		default:
			break;
		}

		if (obj == nullptr)
		{
			return;
		}

		for (unsigned int objectIdx = 0; objectIdx < OBJECT_NUM; objectIdx++)
		{
			if (s_objectPool[objectIdx] == nullptr)
			{
				s_objectPool[objectIdx] = obj;
				return;
			}
		}

		delete obj; // if objectPool was full, new object cannot be created.
	}

	return;
}

int main(void)
{
	while (true)
	{
		InputProcess();
		Action();

		system("cls");
		Draw();

		Sleep(100);
	}
	return 0;
}