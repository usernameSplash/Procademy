#include <cstdio>
#include <cstddef>
#include <iostream>

void PrintEachBitOfNumber()
{
	uint8_t number;
	std::cout << "Input Number: ";
	scanf("%hhu", &number);

	uint8_t curBit = 1 << 7; //MSB of uint8

	printf("%d의 바이너리: ", number);

	while (curBit != 0)
	{
		if (curBit & number) {
			printf("%d", 1);
		}
		else {
			printf("%d", 0);
		}

		curBit = curBit >> 1;
	}

	return;
}

int main()
{
	PrintEachBitOfNumber();
}
