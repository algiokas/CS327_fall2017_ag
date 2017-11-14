#ifndef PQUEUE_H 

#define PQUEUE_H

#include <vector>

template <class T>
class PQueue
{
	struct node {
		int priority;
		T data;
	};

public:
	PQueue() : size(0) {}
	~PQueue();

	bool is_empty();
	void insert(int prio, T data);
	T remove_min();
	T peek();
	void print_queue();

private:


	std::vector<node> nodes;
	int size;

	inline int parent(int idx) { return (idx - 1) / 2; }
	inline int left_child(int idx) { return (2 * idx) + 1; }
	inline int right_child(int idx) { return (2 * idx) + 2; }

	void percolate_up(int node_index);
	void percolate_down(int node_index);
	int min_priority(int idxA, int idxB);
};

#endif
