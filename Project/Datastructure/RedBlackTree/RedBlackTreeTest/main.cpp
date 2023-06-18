#include "RedBlackTree.h"

#include <thread>

using namespace std;
using namespace MyDataStructure;

int main(void)
{
	RedBlackTree tree;

	tree.Insert(50);
	tree.Insert(25);
	tree.Insert(75);
	tree.Insert(100);
	tree.Insert(125);
	tree.Insert(12);
	tree.Insert(37);
	tree.Insert(45);
	tree.Insert(60);
	tree.PrintTree();

	tree.Insert(65);
	system("cls");
	tree.PrintTree();

	system("pause");
}