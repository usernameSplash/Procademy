#include <cstdio>
#include <chrono>
using namespace std;

int matrix[20240][20240] = {};

void Test1()
{
	auto start = chrono::high_resolution_clock::now();

	for (int column = 0; column < 20240; ++column)
	{
		for (int row = 0; row < 20240; ++row)
		{
			matrix[column][row] = 1;
		}
	}

	auto end = chrono::high_resolution_clock::now();
	auto elapsed = end - start;
	auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(elapsed);
	printf("%lld\n", elapsedTime.count());
}

void Test2()
{
	auto start = chrono::high_resolution_clock::now();

	for (int row = 0; row < 20240; ++row)
	{
		for (int column = 0; column < 20240; ++column)
		{
			matrix[column][row] = 1;
		}
	}

	auto end = chrono::high_resolution_clock::now();
	auto elapsed = end - start;
	auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(elapsed);
	printf("%lld\n", elapsedTime.count());
}

int main()
{
	int a;
	a = 0;

	unsigned int p = ((unsigned int)&a) & (0xffffffc0);

	printf("%#010x / %#010x\n", &a, p);

	Test1();
	Test2();

	return 0;
}