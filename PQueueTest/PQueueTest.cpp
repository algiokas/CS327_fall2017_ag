// PQueueTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PQueue.h"


int main()
{
	PQueue<int> p;

	p.insert(1, 10);
	p.insert(2, 9);
	p.insert(3, 8);
	p.insert(4, 7);

	p.print_queue();

    return 0;
}

