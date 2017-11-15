#include "PQueue.h"

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
	int i;
	for (i = 0; i < size; i++) {
		std::cout << " (" << nodes[i].priority << ", " << nodes[i].data << ")";
	}
	std::cout << std::endl;
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
	int left = left_child(idx);
	int right = right_child(idx);
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
