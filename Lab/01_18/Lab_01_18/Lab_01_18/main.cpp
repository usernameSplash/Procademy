#include <cstdio>

/*
	IP ���������� ���� �̻��� ������ �����ϴ� �ڵ�.
	Test �Լ��� return�� ����� �� ������ �߻��Ѵ�.
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