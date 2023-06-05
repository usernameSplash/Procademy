#include "BinarySearchTree.h"

#include <cstdio>

using namespace MyDataStructure;

#define Assert(condition) \
	do \
	{ \
		if(!(condition)) \
		{ \
			wprintf(L"%s in %d\n", L#condition, __LINE__); \
		} \
	} while (0)

int main(void)
{
	BinarySearchTree tree {};

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

	tree.PrintTree();

	return 0;
}