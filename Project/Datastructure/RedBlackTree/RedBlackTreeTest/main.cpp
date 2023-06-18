#include "RedBlackTree.h"

#include <thread>
#include <cstdlib>

using namespace std;
using namespace MyDataStructure;

void Test(void);
void Test1(void);

int main(void)
{
	Test();
	system("pause");
	//RedBlackTree tree;

	//tree.Insert(50);
	//tree.Insert(25);
	//tree.Insert(75);
	//tree.Insert(100);
	//tree.Insert(125);
	//tree.Insert(12);
	//tree.Insert(37);
	//tree.Insert(45);
	//tree.Insert(60);
	//tree.PrintTree();

	//tree.Insert(65);
	//system("cls");
	//tree.PrintTree();

	//system("pause");
}

void Test(void)
{
	RedBlackTree tree;

	srand(2000);

	for (int iCnt = 0; iCnt < 50; iCnt++)
	{
		tree.Insert(rand()%100);
		system("cls");
		tree.PrintTree();
		this_thread::sleep_for(0.1s);
	}
}

void Test1(void)
{
	RedBlackTree tree;

	for (int iCnt = 1; iCnt <= 50; iCnt++)
	{
		tree.Insert(iCnt);
		system("cls");
		tree.PrintTree();
		this_thread::sleep_for(0.1s);
	}
}