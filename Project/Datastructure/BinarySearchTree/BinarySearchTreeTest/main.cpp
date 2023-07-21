#include "BinarySearchTree.h"

#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace MyDataStructure;
using namespace std;

#define Assert(condition) \
	do \
	{ \
		if(!(condition)) \
		{ \
			wprintf(L"%s in %d\n", L#condition, __LINE__); \
		} \
	} while (0)

void Test0(void);
void Test1(void);

int wmain(void)
{
	wprintf(L"Test 0 Start\n");
	Test0();
	wprintf(L"Test 0 End\n");

	wprintf(L"Test 1 Start\n");
	Test1();
	wprintf(L"Test 1 End\n");


	return 0;
}

void Test0(void)
{
	BinarySearchTree tree;

	Assert(tree.Insert(1));
	Assert(tree.Contain(1));
	Assert(tree.Size() == 1);

	Assert(tree.Delete(1));
	Assert(!tree.Contain(1));
	Assert(tree.Size() == 0);

	Assert(tree.Insert(2));
	Assert(tree.Contain(2));
	Assert(tree.Size() == 1);

	Assert(tree.Insert(3));
	Assert(tree.Contain(3));
	Assert(tree.Size() == 2);

	Assert(tree.Insert(4));
	Assert(tree.Contain(4));
	Assert(tree.Size() == 3);

	tree.PrintTree();

	Assert(tree.Delete(2));
	Assert(tree.Size() == 2);

	tree.PrintTree();

	Assert(tree.Delete(4));
	Assert(tree.Size() == 1);

	tree.PrintTree();

	Assert(tree.Delete(3));
	Assert(tree.Size() == 0);

}

void Test1(void)
{
	BinarySearchTree tree {};
	int* array = new int[10];

	Assert(tree.Insert(5));
	Assert(tree.Insert(8));
	Assert(tree.Insert(1));
	Assert(tree.Insert(3));
	Assert(tree.Insert(2));
	Assert(tree.Insert(7));
	Assert(tree.Insert(9));
	Assert(tree.Insert(6));
	Assert(tree.Insert(10));
	Assert(tree.Insert(4));

	tree.GetArray(array, 10);

	for (int i = 0; i < 10; i++)
	{
		Assert(array[i] == (i + 1));
	}

	tree.PrintTree();

	Assert(tree.Size() == 10);

	for (int i = 0; i < 10; i++)
	{
		Assert(tree.Delete(i + 1));
		Assert(tree.Size() == (10 - (i + 1)));

		tree.PrintTree();
	}

	delete[] array;

	return;
}