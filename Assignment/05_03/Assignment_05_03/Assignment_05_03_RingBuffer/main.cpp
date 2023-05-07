#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "RingBuffer.h"

using namespace MyDataStructure;

#define TEST_STR_LEN 81
#define MY_RANDOM_SEED 3000

char g_TestString[TEST_STR_LEN + 1] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
char* g_TestStringPtr;

int main(void)
{
	RingBuffer r;
	
	g_TestStringPtr = g_TestString;

	srand(MY_RANDOM_SEED);

	size_t testBigCnt = 0;
	size_t testCnt = 0;

	while (1)
	{
		char enqueueBuffer[3][82];
		char dequeueBuffer[3][82];
	
		size_t enqueueSize[3];
		size_t peekSize[3];
		size_t dequeueSize[3];

		// Enqueue 3 times;
		for (size_t iCnt = 0; iCnt < 3; iCnt++)
		{
			int randomLen = rand() % TEST_STR_LEN + 1;
			int remainSize = (int)(TEST_STR_LEN - (g_TestStringPtr - g_TestString));

			if (randomLen > remainSize)
			{
				strncpy(enqueueBuffer[iCnt], g_TestStringPtr, remainSize);
				enqueueBuffer[iCnt][remainSize] = '\0';
				strncat(enqueueBuffer[iCnt], g_TestString, (randomLen - remainSize));
				enqueueBuffer[iCnt][randomLen] = '\0';

				enqueueSize[iCnt] = r.Enqueue(enqueueBuffer[iCnt], randomLen);
				g_TestStringPtr = g_TestString + (randomLen - remainSize);
			}
			else
			{
				strncpy(enqueueBuffer[iCnt], g_TestStringPtr, randomLen);
				enqueueBuffer[iCnt][randomLen] = '\0';

				enqueueSize[iCnt] = r.Enqueue(enqueueBuffer[iCnt], randomLen);
				g_TestStringPtr = g_TestStringPtr + randomLen;
			}
		}

		// Dequeue 3 times
		for (size_t iCnt = 0; iCnt < 3; iCnt++)
		{
			peekSize[iCnt] = r.Peek(dequeueBuffer[iCnt], enqueueSize[iCnt]);
			dequeueSize[iCnt] = r.Dequeue(enqueueSize[iCnt]);

			assert((enqueueSize[iCnt] == peekSize[iCnt]) && (peekSize[iCnt] == dequeueSize[iCnt]));

			dequeueBuffer[iCnt][enqueueSize[iCnt]] = '\0';
		}

		for (size_t iCnt = 0; iCnt < 3; iCnt++)
		{
			//printf("%s\n", enqueueBuffer[iCnt]);
			//printf("%s\n", dequeueBuffer[iCnt]);

			assert((enqueueSize[iCnt] == peekSize[iCnt]) && (peekSize[iCnt] == dequeueSize[iCnt]));
			assert(strcmp(enqueueBuffer[iCnt], dequeueBuffer[iCnt]) == 0);
		}

		printf("Test No. %zu - %zu : Success\n", testBigCnt, testCnt);

		if (testCnt == SIZE_MAX)
		{
			testCnt = 0;
			testBigCnt++;
		}

		testCnt++;
	}

	return 0;
}

