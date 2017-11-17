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
		T data;
        if (size > 0) {
			data = nodes.front().data;
			if (size > 1) {
				nodes[0] = nodes.back();
			}
			nodes.pop_back();
			percolate_down(0);
			size--;
        }
        return data;
    }
	T remove_shift()
	{
		int front_prio = nodes.front().priority;
		reduce_priority(front_prio);
		return remove_min();
	}
    T peek()
    {
        return nodes.front().data;
    }
	void print_queue();
	void clear_queue();

private:


	std::vector<node> nodes;
	size_t size;

	inline int parent(int idx) { return (idx - 1) / 2; }
	inline int left_child(int idx) { return (2 * idx) + 1; }
	inline int right_child(int idx) { return (2 * idx) + 2; }

	void percolate_up(int node_index);
	void percolate_down(int node_index);
	int min_priority(int idxA, int idxB);

	void reduce_priority(int diff);
};


template<class T>
PQueue<T>::~PQueue()
{
	//empty
}

template<class T>
bool PQueue<T>::is_empty()
{
	return size == 0;
}

template<class T>
void PQueue<T>::print_queue()
{
	for (size_t i = 0; i < size; i++) {
		std::cout << " (" << nodes[i].priority << ", " << nodes[i].data << ")";
	}
	std::cout << std::endl;
}

template<class T>
void PQueue<T>::clear_queue()
{
	nodes.erase();
	size = 0;
}

template<class T>
void PQueue<T>::percolate_up(int node_index)
{
	int idx = node_index;
	if (idx == 0) {
		return;
	}
	if (nodes.at(idx).priority < nodes.at(parent(idx)).priority) {
		node temp = nodes[idx];
		nodes[idx] = nodes[parent(idx)];
		nodes[parent(idx)] = temp;

		percolate_up(parent(idx));
	}
}

template<class T>
void PQueue<T>::percolate_down(int node_index)
{
	int idx = node_index;
	unsigned int left = left_child(idx);
	unsigned int right = right_child(idx);
	if (right >= size || left >= size) {
		return;
	}

	if (nodes[idx].priority > nodes[left].priority
		|| nodes[idx].priority > nodes[right].priority)
	{
		int minChild = min_priority(left, right);
		node temp = nodes[idx];
		nodes[idx] = nodes[minChild];
		nodes[minChild] = temp;

		percolate_down(minChild);
	}
}

template<class T>
int PQueue<T>::min_priority(int idxA, int idxB)
{

	return (nodes[idxA].priority < nodes[idxB].priority) ? idxA : idxB;
}

template<class T>
inline void PQueue<T>::reduce_priority(int diff)
{
	for (size_t i = 0; i < size; i++) {
		nodes.at(i).priority -= diff;
	}
}

#endif
