#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <Windows.h>

const int TIME_NUM = 10;

enum class eResultType
{
	GREAT,
	GOOD,
	NOTGOOD,
	BAD,
	FAIL
};

eResultType g_result[TIME_NUM];
DWORD g_resultRawData[TIME_NUM];

DWORD g_time[TIME_NUM] = { 5, 10, 14, 17, 20, 25, 29, 31, 33, 37};
DWORD g_curIndex = 0;

DWORD g_startTime;
DWORD g_curTime;

bool g_Running = true;

DWORD GetCurrentProcessTime()
{
	DWORD curTimeTemp = timeGetTime();
	DWORD curTime;

	if (curTimeTemp >= g_startTime)
	{
		curTime = curTimeTemp - g_startTime;
	}
	else
	{
		curTime = (MAXDWORD - g_startTime) + curTimeTemp;
	}

	return curTime;
}

void Update()
{
	g_curTime = GetCurrentProcessTime();

	long long result = (long long)g_curTime - (long long)(g_time[g_curIndex]) * 1000;

	if (result >= 1000)
	{
		g_result[g_curIndex] = eResultType::FAIL;
	}
	else if (_kbhit())
	{
		int getchBuffer = _getch();

		if (abs(result) >= 1000)
		{
			g_result[g_curIndex] = eResultType::FAIL;
		}
		else
		{
			g_result[g_curIndex] = static_cast<eResultType>((abs(result) / 250));
		}
	}
	else
	{
		return;
	}

	g_resultRawData[g_curIndex] = g_curTime;

	g_curIndex++;

	if (g_curIndex >= 10)
	{
		g_Running = false;
	}
}

void Render()
{
	DOUBLE curTimeDisplay;
	curTimeDisplay = g_curTime / 1000.0;

	system("cls");

	printf("%02.3f Sec\n\n", curTimeDisplay);

	for (DWORD iCnt = 0; iCnt < TIME_NUM; iCnt++)
	{
		printf("%02d Sec: ", g_time[iCnt]);

		char resultText[10] = "";
		if (iCnt < g_curIndex)
		{
			switch (g_result[iCnt])
			{
			case eResultType::GREAT:
				strncpy(resultText, "GREAT", 6);
				break;
			case eResultType::GOOD:
				strncpy(resultText, "GOOD", 5);
				break;
			case eResultType::NOTGOOD:
				strncpy(resultText, "NOT GOOD", 8);
				break;
			case eResultType::BAD:
				strncpy(resultText, "BAD", 4);
				break;
			case eResultType::FAIL:
				strncpy(resultText, "FAIL", 5);
				break;
			default:
				break;
			}
		}

		printf("%s\n", resultText);
	}
}

void TimingGame()
{
	while (g_Running)
	{
		Update();
		Render();
	}

	printf("\nGame Ended. Check Your Result.\n");
	system("pause");
}

int main()
{
	timeBeginPeriod(1);	//set time-resolution to highest level.
	g_startTime = timeGetTime();
	TimingGame();

	return 0;
}