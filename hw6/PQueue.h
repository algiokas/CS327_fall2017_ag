#ifndef PQUEUE_H

#define PQUEUE_H

struct node
{
	int priority;
	void *data;
	struct node *next;
	struct node *prev;
};


class PQueue
{
private:
	node *front;
	int element_size;
	int size;

public:
	PQueue(int eSize);
	~PQueue();

	bool is_empty();
	void insert();
	void *remove_min();
	void *peek_min();
	void print();
};

#endif