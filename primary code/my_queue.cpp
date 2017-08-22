#include "my_queue.h"


my_queue::my_queue()
{
	Pre_node = NULL;
    time ( &cur_time );

}


my_queue::~my_queue()
{
}
void my_queue::set_node(Node* node)
{
	Pre_node = new Node; 
	Pre_node = node;
}

Node* my_queue::get_node()
{
	return Pre_node;
}
