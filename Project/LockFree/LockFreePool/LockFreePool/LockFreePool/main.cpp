#include "LockFreePool.h"

#include <cstdio>
int wmain(void)
{
	__int64 a = 0xffffffffffffffff;
	wprintf(L"%lld\n", a);

	__int64 b = a << 47;
	wprintf(L"%lld\n", b);

	__int64 c = ~b;
	wprintf(L"%lld\n", c);

	__int64 d = a & c;
	wprintf(L"%lld\n", d);

	__int64 e = 0x00007fffffffffff;
	wprintf(L"%lld\n", e);

	__int64 f = GET_PTR(e);
	wprintf(L"%lld\n", f);
}