#include <cstdio>

void Test()
{
	int message;
	int x;

	message = 10;
	x = 0xffffffff;

	switch (message)
	{
	case 10:
		x = 0;
		break;

	case 11:
		x = 0;
		break;

	case 12:
		x = 0;
		break;

	case 13:
		x = 0;
		break;

	case 14:
		x = 0;
		break;

	case 16:
		x = 0;
		break;

	case 17:
		x = 0;
		break;

	case 18:
		x = 0;
		break;

	case 19:
		x = 0;
		break;

	case 20:
		x = 0;
		break;

	default:
		break;
	}
}

int main()
{
	Test();

	return 0;
}