#include "linkedqueue.h"

linkedqueue::linkedqueue()
{
	Front = NULL;
	Rear = NULL;
	Count = 0;
}


linkedqueue::~linkedqueue()
{ 
}
void  linkedqueue::Enqueue(linkedqueue* q_list, my_queue* New_queue)
{
	if (q_list->Front == NULL)
	{
		q_list->Front = New_queue;
		q_list->Rear = New_queue;
		q_list->Count++;
	}
	else
	{
		q_list->Rear->next_queue = New_queue;
		q_list->Rear = New_queue;
		q_list->Count++;
	}
}

my_queue* linkedqueue::Dequeue(linkedqueue* q_list)
{
	my_queue* Front = q_list->Front;

	if (q_list->Front->next_queue == NULL)
	{
		q_list->Front = NULL;
		q_list->Rear = NULL;
	}
	else
	{
		q_list->Front = q_list->Front->next_queue;
	}

	q_list->Count--;

	return Front;
}

int  linkedqueue::IsEmpty(linkedqueue* Queue)
{
	return (Queue->Front == NULL);
}
