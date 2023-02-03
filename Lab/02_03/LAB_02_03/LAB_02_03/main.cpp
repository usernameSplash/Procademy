#include <cstdio>
#include <cstdlib>
#include <Windows.h>

int main()
{
	int* buffer = (int*)malloc(sizeof(int) * 100000000);

	for (int i = 0; i < 100000000; i++)
	{
		printf("%d\n", i);
		buffer[i] = 1;
	}
	Sleep(10000);

	free(buffer);

	return 0;
}