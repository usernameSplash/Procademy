#include "MemoryPool.h"

#include <cassert>
#include <cstdio>

using namespace MyDataStructure;

void Test0(void);
void Test1(void);
void Test2(void);

typedef struct Data
{
	int a;
	int b;
} Data;

int main(void)
{
	printf("Test 0 Start\n");
	Test0();
	printf("Test 0 End\n");

	printf("Test 1 Start\n");
	Test1();
	printf("Test 1 End\n");

	printf("Test 2 Start\n");
	Test2();
	printf("Test 2 End\n");

	return 0;
}

void Test0(void)
{
	MemoryPool<Data> m { 100, false };

	size_t capa = m.Capacity();
	size_t size = m.Size();
	
	assert(capa == 100);
	assert(size == 100);

	Data* ptr = m.Alloc();
	ptr->a = 101;
	ptr->b = 202;

	size = m.Size();
	assert(size == 99);

	m.Free(ptr);
	size = m.Size();
	assert(size == 100);

	Data* ptr2 = m.Alloc();
	assert(ptr2->a == 101);
	assert(ptr2->b == 202);

	return;
}

void Test1(void)
{
	MemoryPool<Data> m {};

	size_t capa = m.Capacity();
	size_t size = m.Size();
	assert(capa == 0);
	assert(size == 0);

	Data* ptr1 = m.Alloc();
	capa = m.Capacity();
	size = m.Size();
	assert(capa == 1);
	assert(size == 0);

	Data* ptr2 = m.Alloc();
	capa = m.Capacity();
	size = m.Size();
	assert(capa == 2);
	assert(size == 0);

	ptr1->a = 101;
	ptr1->b = 202;
	ptr2->a = 303;
	ptr2->b = 404;

	m.Free(ptr1);
	m.Free(ptr2);
	capa = m.Capacity();
	size = m.Size();
	assert(capa == 2);
	assert(size == 2);

	Data* ptr3 = m.Alloc();
	Data* ptr4 = m.Alloc();

	assert(ptr3->a == 303);
	assert(ptr3->b == 404);
	assert(ptr4->a == 101);
	assert(ptr4->b == 202);

	return;
}

void Test2(void)
{
	Data* dataPtrArray[100];
	MemoryPool<Data> m;

	size_t capa;
	size_t size;

	for (size_t dataCnt = 0; dataCnt < 100; dataCnt++)
	{
		dataPtrArray[dataCnt] = m.Alloc();

		dataPtrArray[dataCnt]->a = (int)(dataCnt * 100);
		dataPtrArray[dataCnt]->b = (int)(dataCnt * 200);
	}

	capa = m.Capacity();
	size = m.Size();
	assert(capa == 100);
	assert(size == 0);

	for (size_t dataCnt = 0; dataCnt < 100; dataCnt++)
	{
		m.Free(dataPtrArray[dataCnt]);
	}

	capa = m.Capacity();
	size = m.Size();
	assert(capa == 100);
	assert(size == 100);

	for (size_t iCnt = 0; iCnt < 100000; iCnt++)
	{
		for (size_t dataCnt = 0; dataCnt < 100; dataCnt++)
		{
			Data* dataPtr = m.Alloc();

			if (iCnt % 2 == 0)
			{
				assert(dataPtr->a == int((100 - dataCnt - 1) * 100));
				assert(dataPtr->b == int((100 - dataCnt - 1) * 200));
			}
			else
			{
				assert(dataPtr->a == int(dataCnt * 100));
				assert(dataPtr->b == int(dataCnt * 200));
			}

			dataPtrArray[dataCnt] = dataPtr;
		}

		for (size_t dataCnt = 0; dataCnt < 100; dataCnt++)
		{
			m.Free(dataPtrArray[dataCnt]);
		}

		capa = m.Capacity();
		size = m.Size();
		assert(capa == 100);
		assert(size == 100);
	}

	for (size_t iCnt = 0; iCnt < 100000; iCnt++)
	{
		Data* dataPtr = m.Alloc();

		assert(dataPtr->a == 9900);
		assert(dataPtr->b == 19800);

		m.Free(dataPtr);
	}

	capa = m.Capacity();
	size = m.Size();
	assert(capa == 100);
	assert(size == 100);

	return;
}