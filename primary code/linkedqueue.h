#pragma once
#include "my_queue.h"

class Node;
class my_queue;
class linkedqueue
{
public: 

	my_queue* Front;
	my_queue* Rear;
	int   Count;

	linkedqueue();
	~linkedqueue();


	void  Enqueue(linkedqueue* q_list, my_queue* New_queue);
	my_queue* Dequeue(linkedqueue* q_list);
	int IsEmpty(linkedqueue* q_list);
};

