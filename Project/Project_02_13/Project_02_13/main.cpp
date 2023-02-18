#define  _WIN32_WINNT 0x0600

#include <cstdio>
#include <memory.h>
#include <Windows.h>
#include <sysinfoapi.h>
#include <timeapi.h>

#include "Console.h"
#include "Logger.h"
#include "LoadData.h"

#pragma comment(lib, "Winmm.lib") // for using timeGetTime function

//--------------------------------------------------------------------
// ȭ�� �������� ���ֱ� ���� ȭ�� ����.
// ������ ����Ǵ� ��Ȳ�� �Ź� ȭ���� ����� ����� ���, ����� ���,
// �ϰ� �Ǹ� ȭ���� �������� �Ÿ��� �ȴ�.
//
// �׷��Ƿ� ȭ��� �Ȱ��� ũ���� �޸𸮸� �Ҵ��� ������ ȭ�鿡 �ٷ� �����ʰ�
// �޸�(����)�� �׸��� �׸��� �޸��� ȭ���� �״�� ȭ�鿡 ����ش�.
//
// �̷��� �ؼ� ȭ���� �Ź� �����, �׸���, �����, �׸��� ���� �ʰ�
// �޸�(����)���� �׸��� ȭ�鿡 �׸��� �۾��� �ϰ� �Ǿ� �������� ��������.
//
// ������ �� �� �������� NULL �� �־� ���ڿ��μ� ó���ϸ�, 
// ���������� printf �� ���� ���̴�.
//
// for ( N = 0 ~ height )
// {
// 	  cs_MoveCursor(0, N);
//    printf(szScreenBuffer[N]);
// }
//
// �ٹٲ޿� printf("\n") �� ���� �ʰ� Ŀ����ǥ�� �̵��ϴ� ������
// ȭ���� �� ���� ����ϰ� �ٹٲ��� �ϸ� 2ĭ�� �������ų� ȭ���� �и� �� �����Ƿ�
// �� �� ��¸��� ��ǥ�� ������ �̵��Ͽ� Ȯ���ϰ� ����Ѵ�.
//--------------------------------------------------------------------
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];


//--------------------------------------------------------------------
// GetAsyncKeyState(int iKey)  #include <Windows.h>
//
// ������ API �� Ű���尡 ���ȴ����� Ȯ���Ѵ�.
// ���ڷ� Ű���� ��ư�� ���� ������ ���� ������ �ش� Ű�� ���ȴ��� (���ȴ����� �ִ���) �� Ȯ�� ���ش�.
// ��� Ű������ Ȯ���� �����ϰ�, ��� üũ�� �ǹǷ� ���ӿ����� ���� ����.
//
// Virtual-Key Codes
//
// VK_SPACE / VK_ESCAPE / VK_LEFT / VK_UP / Ű���� ���ڴ� �빮�� �ƽ�Ű �ڵ�� ����.
// winuser.h ���Ͽ� ���� ���� ������ �Ǿ� �ִ�.
//
//
// GetAsyncKeyState(VK_LEFT) ȣ��� �������
//
// 0x0001  > *���� üũ ���� �������� ����
// 0x8000  > ���� ��������
// 0x8001  > *���� üũ ���� �������� �ְ� ���ݵ� ���� ����
//
// * ���� üũ��°� ������ GetAsyncKeyState �� ȣ���� ���� �� �Ѵ�.
// 
// 10������ ¥�� �����̶�� 1�ʿ� 10ȸ�� Ű üũ�� �ϰ� �ǹǷ� üũ ������ 20ms �� �ȴ�.
// ���� Ŀ�ǵ� �Է��� �ʿ��� ���ӿ����� 20ms �̳��� �������� Ű�Է��� �ִٸ� üũ���� ���ϴ� Ű �Է��� �߻� �� �� �ִ�.
// �׷��� 0x0001 ��Ʈ�� ���� ó���� �ʿ��ϴ�.
//


//--------------------------------------------------------------------
// ������ ������ ȭ������ ����ִ� �Լ�.
//
// ����,�Ʊ�,�Ѿ� ���� szScreenBuffer �� �־��ְ�, 
// 1 �������� ������ �������� �� �Լ��� ȣ���Ͽ� ���� -> ȭ�� ���� �׸���.
//--------------------------------------------------------------------
void Buffer_Flip(void);
//--------------------------------------------------------------------
// ȭ�� ���۸� �����ִ� �Լ�
//
// �� ������ �׸��� �׸��� ������ ���۸� ���� �ش�. 
// �ȱ׷��� ���� �������� �ܻ��� �����ϱ�
//--------------------------------------------------------------------
void Buffer_Clear(void);

//--------------------------------------------------------------------
// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
//
// �Է� ���� X,Y ��ǥ�� �ƽ�Ű�ڵ� �ϳ��� ����Ѵ�. (���ۿ� �׸�)
//--------------------------------------------------------------------
void Sprite_Draw(int iX, int iY, char chSprite);


int g_iX = 0;

int main(void)
{
	const DWORD TIME_PER_FRAME = 1000 / 60;
	timeBeginPeriod(1);

	DWORD prevTime;
	DWORD curTime;
	DWORD deltaTime;

	int iX = 0;  
	int iY = 0;

	cs_Initial();
	PrintLog("Program Start", error_t::NOTE);

	LoadAllStageFileName();
	curTime = timeGetTime();
	while (1)
	{

		iX++;
		iX = iX % dfSCREEN_WIDTH;
		iY++;
		iY = iY % dfSCREEN_HEIGHT;

		Buffer_Clear();
		Sprite_Draw(iX, iY, 'A');
		Buffer_Flip();

		prevTime = curTime;
		curTime = timeGetTime();

		if (curTime < prevTime)
		{
			deltaTime = MAXDWORD - prevTime + curTime;
		}
		else
		{
			deltaTime = curTime - prevTime;
		}

		if (deltaTime < TIME_PER_FRAME)
		{
			Sleep(TIME_PER_FRAME - deltaTime);
		}
	}

	PrintLog("Program End Successfully", error_t::NOTE);
	return 0;
}



//--------------------------------------------------------------------
// ������ ������ ȭ������ ����ִ� �Լ�.
//
// ����,�Ʊ�,�Ѿ� ���� szScreenBuffer �� �־��ְ�, 
// 1 �������� ������ �������� �� �Լ��� ȣ���Ͽ� ���� -> ȭ�� ���� �׸���.
//--------------------------------------------------------------------
void Buffer_Flip(void)
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		printf(szScreenBuffer[iCnt]);
	}
}


//--------------------------------------------------------------------
// ȭ�� ���۸� �����ִ� �Լ�
//
// �� ������ �׸��� �׸��� ������ ���۸� ���� �ش�. 
// �ȱ׷��� ���� �������� �ܻ��� �����ϱ�
//--------------------------------------------------------------------
void Buffer_Clear(void)
{
	int iCnt;
	memset(szScreenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][dfSCREEN_WIDTH - 1] = '\0';
	}

}

//--------------------------------------------------------------------
// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
//
// �Է� ���� X,Y ��ǥ�� �ƽ�Ű�ڵ� �ϳ��� ����Ѵ�. (���ۿ� �׸�)
//--------------------------------------------------------------------
void Sprite_Draw(int iX, int iY, char chSprite)
{
	if (iX < 0 || iY < 0 || iX >= dfSCREEN_WIDTH - 1 || iY >= dfSCREEN_HEIGHT)
		return;

	szScreenBuffer[iY][iX] = chSprite;
}




