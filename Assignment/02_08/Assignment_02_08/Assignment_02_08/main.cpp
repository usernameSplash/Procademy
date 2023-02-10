#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_WORD 23

char g_Dict[MAX_WORD][2][32] = {
	{"i", "나"},
	{"you", "너"},
	{"me", "나를"},
	{"he", "그"},
	{"she", "그녀"},
	{"man", "남자"},
	{"woman", "여자"},
	{"boy", "소년"},
	{"girl", "소녀"},
	{"school", "학교"},
	{"hate", "증오하다"},
	{"this", "이것"},
	{"that", "저것"},
	{"a", "하나의"},
	{"an", "하나의"},
	{"help", "도와주다"},
	{"hello", "안녕"},
	{"world", "세계"},
	{"love", "사랑"},
	{"game", "게임"},
	{"go", "가다"},
	{"crazy", "미친"},
	{"are", "는"}
};

void Trans(char* originString, char* outString, int* outWordCount);
char* FindWord(const char* word);

int main(void)
{
	char inputString[128];
	char outputString[256] = "";
	int wordCount;

	printf("63자 이하로 영어 문장을 입력하세요.\n");
	
	fgets(inputString, 128, stdin);
	strlwr(inputString);

	Trans(inputString, outputString, &wordCount);

	printf("\n - 총 %d개의 단어가 번역되었습니다. \n", wordCount);
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