#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_WORD 23

char g_Dict[MAX_WORD][2][32] = {
	{"i", "��"},
	{"you", "��"},
	{"me", "����"},
	{"he", "��"},
	{"she", "�׳�"},
	{"man", "����"},
	{"woman", "����"},
	{"boy", "�ҳ�"},
	{"girl", "�ҳ�"},
	{"school", "�б�"},
	{"hate", "�����ϴ�"},
	{"this", "�̰�"},
	{"that", "����"},
	{"a", "�ϳ���"},
	{"an", "�ϳ���"},
	{"help", "�����ִ�"},
	{"hello", "�ȳ�"},
	{"world", "����"},
	{"love", "���"},
	{"game", "����"},
	{"go", "����"},
	{"crazy", "��ģ"},
	{"are", "��"}
};

void Trans(char* originString, char* outString, int* outWordCount);
char* FindWord(const char* word);

int main(void)
{
	char inputString[128];
	char outputString[256] = "";
	int wordCount;

	printf("63�� ���Ϸ� ���� ������ �Է��ϼ���.\n");
	
	fgets(inputString, 128, stdin);
	strlwr(inputString);

	Trans(inputString, outputString, &wordCount);

	printf("\n - �� %d���� �ܾ �����Ǿ����ϴ�. \n", wordCount);
	printf("# %s \n", outputString);

	return 0;
}

void Trans(char* originString, char* outString, int* outWordCount)
{
	const size_t ORIGIN_LENGTH = strlen(originString);
	char* engWord;
	char* korWord;

	engWord = originString;

	*outWordCount = 0;

	for (size_t iCnt = 0; iCnt < ORIGIN_LENGTH + 1; iCnt++)
	{
		if (originString[iCnt] == ' ' || originString[iCnt] == '\0' || originString[iCnt] == '\n')
		{
			originString[iCnt] = '\0';

			korWord = FindWord(engWord);

			if (korWord == NULL)
			{
				strcat(outString, engWord);
			}
			else
			{
				strcat(outString, korWord);
				(*outWordCount)++;
			}

			engWord = &originString[iCnt + 1];

			strcat(outString, " ");
		}
	}
}

char* FindWord(const char* word)
{
	for (size_t iCnt = 0; iCnt < MAX_WORD; iCnt++)
	{
		if (strcmp(g_Dict[iCnt][0], word) == 0)
		{
			return g_Dict[iCnt][1];
		}
	}

	return NULL;
}