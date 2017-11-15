#ifndef PQUEUE_H 

#define PQUEUE_H

#include <vector>
#include <iostream>

template <class T>
class PQueue
{
	class node {
        public:
		    int priority;
		    T data;
	};

public:
	PQueue() : size(0) {}
	~PQueue();

	bool is_empty();
	void insert(int prio, T data)
    {
        node new_node = node();
        new_node.priority = prio;
        new_node.data = data;
        nodes.push_back(new_node);
        percolate_up(size++);
    }

	T remove_min()
    {
        if (size < 1) {
            std::cout << "empty queue";
            return -1;
        }
        int value = nodes.front().data;
        if (size > 1) {
            nodes[0] = nodes.back();
            nodes.pop_back();
            percolate_down(0);
        }
        size--;
        return value;
    }
    T peek()
    {
        return nodes.front().data;
    }
	void print_queue();

private:


	std::vector<node> nodes;
	size_t size;

	inline int parent(int idx) { return (idx - 1) / 2; }
	inline int left_child(int idx) { return (2 * idx) + 1; }
	inline int right_child(int idx) { return (2 * idx) + 2; }

	void percolate_up(int node_index);
	void percolate_down(int node_index);
	int min_priority(int idxA, int idxB);
};

#endif
