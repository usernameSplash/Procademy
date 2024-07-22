#include "LockFreeQueue.h"

#include <cstdio>


int main(void)
{
	LockFreeQueue<int> q;

	while (true)
	{
		q.Enqueue(1);
		int a = q.Dequeue();
		printf("%d", a);
	}
}