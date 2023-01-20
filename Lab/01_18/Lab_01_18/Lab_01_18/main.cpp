#include <cstdio>

/*
	IP 레지스터의 값을 이상한 값으로 변경하는 코드.
	Test 함수의 return이 실행될 때 에러가 발생한다.
*/
void Test(void)
{
	int x;
	int* p;

	x = 0xff;

	p = &x;
	*p = 0;

	p += 3;

	*p = 0;

	return;
}

int main(void)
{
	Test();

	return 0;
}