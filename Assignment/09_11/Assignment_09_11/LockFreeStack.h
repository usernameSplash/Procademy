#pragma once

#include <process.h>
#include <Windows.h>

template<typename T>
class LockFreeStack
{
private:
	struct Node
	{
		T _data;
		Node* _next;
	};

public:
	LockFreeStack()
		: _top(nullptr)
	{
	}

	~LockFreeStack()
	{
		Node* curNode = _top;
		while (curNode != nullptr)
		{
			Node* deleteNode = curNode;
			delete deleteNode;

			curNode = curNode-> _next;
		}
	}

private:
	Node* _top;

public:
	void Push(T data)
	{
		Node* node = new Node;
		node->_data = data;

		Node* top;

		do
		{
			top = _top;
			node->_next = _top;
		} while (!InterlockedCompareExchange64((LONG64*)&_top, (LONG64)node, (LONG64)top));
	}

	T Pop(void)
	{
		T data;
		Node* top;
		Node* next;

		do
		{
			top = _top;
			data = top->_data;
			next = top->_next;
		} while (!InterlockedCompareExchange64((LONG64*)&_top, (LONG64)next, (LONG64)top));

		delete top;

		return data;
	}
};