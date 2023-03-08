#include <iostream>

#include "MemoryTracker.h"

class Base
{
public:
	Base() : a(0), b(0), c(0)
	{
		std::cout << "Base Constructor" << std::endl;
	}
	~Base()
	{
		std::cout << "Base Destructor" << std::endl;
	}

	int a;
	int b;
	int c;
};

class A : public Base
{
public:
	A()
	{
		std::cout << "A Constructror" << std::endl;
	}
	~A()
	{
		std::cout << "A Destructor" << std::endl;
	}
};

int main(void)
{
	A* p1 = new A;

	//delete[] p1;

	return 0;
}