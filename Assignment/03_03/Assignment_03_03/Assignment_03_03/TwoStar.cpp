#include <cstdio>

#include "TwoStar.h"

TwoStar::TwoStar()
{

}

bool TwoStar::Update(void)
{
	x += 2;

	if (x > 79)
	{
		return false;
	}

	return true;
}

void TwoStar::Render(void)
{
	for (unsigned int spaceCount = 0; spaceCount < x; spaceCount++)
	{
		printf(" ");
	}

	printf("**\n");
}