#include "PQueue.h"

#include <iostream>
#include <cstddef>

template<class DType>
int PQueue<DType>::min_priority(int idxA, int idxB)
{
	return (nodes[idxA].priority < nodes[idxB].priority) ? idxA : idxB;
}

template<class DType>
void PQueue<DType>::percolate_up(int node_index)
{
	int idx = node_index;
	if (nodes[idx].priority < nodes[parent(idx)].priority) {
		node temp = nodes[idx];
		nodes[idx] = nodes[parent(idx)];
		nodes[parent(idx)] = temp;

		percolate_up(parent(idx));
	}
}

template<class DType>
void PQueue<DType>::percolate_down(int node_index)
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



template<class DType>
PQueue<DType>::PQueue()
{
	nodes = std::vector<node>();
	size = 0;

}

template<class DType>
PQueue<DType>::~PQueue()
{
	//empty
}

template<typename DType>
bool PQueue<DType>::is_empty()
{
	return size == 0;
}

template<typename DType>
void PQueue<DType>::insert(int prio, DType data)
{
	node new_node = node(prio, data);
	nodes.push_back(new_node);
	//before incrementing the size member, the nodes vector has (size+1) elements, so size is in bounds here
	percolate_up(size++); 
}

template<typename DType>
DType PQueue<DType>::remove_min()
{
	if (size < 1) {
		return NULL;
	}
	DType value = nodes.front().data;
	if (size > 1) {
		nodes[0] = nodes.back();
		nodes.pop_back();
		percolate_down(0);
	}
	return DType();
}

template<typename DType>
DType PQueue<DType>::peek()
{
	return nodes.front().data;
}

template<typename DType>
void PQueue<DType>::print_queue()
{
	int i;
	for (i = 0; i < size; i++) {
		std::cout << " (" << nodes[i].prio << ", " << nodes[i].data << ")";
	}
	std::cout << std::endl;
}
