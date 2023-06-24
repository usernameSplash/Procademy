#include "RedBlackTree.h"

#include <thread>
#include <cstdio>
#include <cassert>
#include <unordered_set>

#define Assert(condition) \
	do \
	{ \
		if(!(condition)) \
		{ \
			wprintf(L"%s in %d\n", L#condition, __LINE__); \
			int* a = nullptr; \
			*a = 0; \
		} \
	} while (0)

using namespace std;
using namespace MyDataStructure;

void Test(void);
void Test1(void);
void Test2(void);

int wmain(void)
{
	Test2();
}

void Test(void)
{
	RedBlackTree tree;

	srand(2000);

	for (int iCnt = 0; iCnt < 50; iCnt++)
	{
		tree.Insert(rand()%100);
		tree.PrintTree();
		this_thread::sleep_for(0.5s);
	}
}

void Test1(void)
{
	RedBlackTree tree;

	srand(2000);

	for (int iCnt = 1; iCnt <= 30; iCnt++)
	{
		tree.Insert(iCnt);
		//tree.PrintTree();
		//this_thread::sleep_for(0.01s);
		Assert(tree.Size() == (iCnt));
		Assert(tree.VerifyTree());
	}

	//this_thread::sleep_for(1.0s);

	size_t size = 30;

	while (tree.Size() > 0)
	{
		int val = rand() % 30 + 1;

		if (tree.Delete(val))
		{
			//tree.PrintTree();
			//this_thread::sleep_for(0.01s);
			size_t treeSize = tree.Size();
			size--;

			bool sizeCheck = treeSize == size;
			bool verifyRedBlackTree = tree.VerifyTree();

			wprintf(L"SizeCheck : %d\n", sizeCheck);
			wprintf(L"VerifyTree : %d\n\n", verifyRedBlackTree);

			Assert(sizeCheck);
			Assert(verifyRedBlackTree);
		}
	}	
}

void Test2(void)
{
	RedBlackTree tree;
	std::unordered_set<int> numSet;
	numSet.reserve(1000000);

	srand(2000);

	size_t treeSize = 0;

	while (tree.Size() < 100000)
	{
		int rand1 = rand();
		int rand2 = rand();

		int randNum = (rand1 << 16) + (rand2 << 8);

		if (tree.Insert(randNum))
		{
			treeSize++;
			Assert(tree.Size() == treeSize);

			if (treeSize % 10 == 0)
			{
				Assert(tree.VerifyTree());
			}

			numSet.insert(randNum);
		}
		wprintf(L"Tree Size : %d\n", treeSize);
	}

	wprintf(L"Insert Success\n");

	for (auto it = numSet.begin(); it != numSet.end(); it++)
	{
		tree.Delete(*it);
		treeSize--;
		Assert(tree.Size() == treeSize);

		if (treeSize % 10 == 0)
		{
			Assert(tree.VerifyTree());
		}

		wprintf(L"Tree Size : %d\n", treeSize);
	}

	Assert(tree.Size() == 0);

	wprintf(L"Delete Success\n");
}