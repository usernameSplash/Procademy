#include <cstdio>
#include <vector>
#include "List.h"

int main(void)
{
	MyDataStructure::List<int> l;

	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_front(4);
	l.push_front(5);

	for (auto it = l.begin(); it != l.end(); ++it)
	{
		printf("%d ", *it);
	}
	std::vector<int> v;
}