#include <iostream>

struct stData
{
	char a;
	short b;
	int c;
	short d;
	int e;
	__int64 f;
	char g;
	__int64 h;
	short i;
};

int main()
{
	std::cout << sizeof(stData) << std::endl;

	return 0;
}