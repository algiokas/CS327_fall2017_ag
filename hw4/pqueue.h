#ifndef PQUEUE_H 

#define PQUEUE_H

#include <vector>

template<class DType>
class PQueue
{
private:
	struct node {
		int priority;
		DType data;
	};

	std::vector<node> nodes;
	int size;

	inline int parent(int idx) { return (idx - 1) / 2; }
	inline int left_child(int idx) { return (2 * idx) + 1; }
	inline int right_child(int idx) { return (2 * idx) + 2; }
	
	void percolate_up(int node_index);
	void percolate_down(int node_index);
	int min_priority(int idxA, int idxB);

public:
	PQueue();
	~PQueue();

	bool is_empty();
	void insert(int prio, DType data);
	DType remove_min();
	DType peek();
	void print_queue();

};

#endif
