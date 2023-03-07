#include <cstdio>

#include "OneStar.h"

OneStar::OneStar()
{

}

bool OneStar::Update(void)
{
	x++;

	if (x > 79)
	{
		return false;
	}

	return true;
}

void OneStar::Render(void)
{
	for (unsigned int spaceCount = 0; spaceCount < x; spaceCount++)
	{
		printf(" ");
	}

	printf("*\n");
}