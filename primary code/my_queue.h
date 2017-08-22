#pragma once
#include "Node.h"
#include <ctime>

class Node;
class my_queue
{
private:
	Node* Pre_node;
  
public:
    time_t cur_time;
	my_queue();
	~my_queue();
	my_queue* next_queue;
	void set_node(Node* node);
	Node* get_node();
};
 
