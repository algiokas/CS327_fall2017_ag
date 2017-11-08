#include "PQueue.h"

#include <iostream>
#include <cstddef>

//template<class DType>
int PQueue::min_priority(int idxA, int idxB)
{

	return (nodes[idxA].priority < nodes[idxB].priority) ? idxA : idxB;
}

//template<class DType>
void PQueue::percolate_up(int node_index)
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

//template<class DType>
void PQueue::percolate_down(int node_index)
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



//template<class DType>
PQueue::PQueue()
{
	nodes = std::vector<node>();
	size = 0;

}

//template<class DType>
PQueue::~PQueue()
{
	//empty
}

//template<typename int>
bool PQueue::is_empty()
{
	return size == 0;
}

void PQueue::insert(int prio, int data)
{
	node new_node = node();
	new_node.priority = prio;
	new_node.data = data;
	nodes.push_back(new_node);
	//before incrementing the size member, the nodes vector has (size+1) elements, so size is in bounds here
	percolate_up(size++); 
}

//template<typename int>
int PQueue::remove_min()
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

//template<typename int>
int PQueue::peek()
{
	return nodes.front().data;
}

//template<typename int>
void PQueue::print_queue()
{
	int i;
	for (i = 0; i < size; i++) {
		std::cout << " (" << nodes[i].priority << ", " << nodes[i].data << ")";
	}
	std::cout << std::endl;
}
