#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>

static unsigned int my_seed;

void my_srand(unsigned int seed)
{
	my_seed = seed;
}

int my_rand(void)
{
	int result;

	result = my_seed * 0x343FD;
	result += 0x269EC3;
	my_seed = result;

	result >>= 0x10;
	result &= 0x7FFF;

	return result;
}

int main(void)
{
	int result;
	int my_result;

	srand(500);
	my_srand(500);

	for (int i = 0; i < 10; ++i)
	{
		result = rand();
		my_result = my_rand();

		assert(result == my_result);
	}

	printf("Success\n");

	return 0;
}