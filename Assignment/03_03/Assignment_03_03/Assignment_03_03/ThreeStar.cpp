#include <cstdio>

#include "ThreeStar.h"

ThreeStar::ThreeStar()
{

}

bool ThreeStar::Update(void)
{
	x += 3;

	if (x > 79)
	{
		return false;
	}

	return true;
}

void ThreeStar::Render(void)
{
	for (unsigned int spaceCount = 0; spaceCount < x; spaceCount++)
	{
		printf(" ");
	}

	printf("***\n");
}