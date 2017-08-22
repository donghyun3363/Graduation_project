#include "Graph.h"
#include<string.h> 

 
Graph::Graph(void)
{
	//boost_graph g; 
	head=NULL;
	  
	memset(arr_url,NULL,5000);
	a_index=0;
	temp_node=NULL; 
    G_queue = new linkedqueue;
}
Graph::~Graph(void)
{    
}
  
Node* Graph::count(Node* i_prior,char* i_url)
{
	if(!head)   
	{
		Node* New = new Node(i_url, i_prior);
		head = New;
		arr_url[a_index]=New;
		a_index++;

		return New;
	}
 
	
	int id = BinarySearch(a_index,i_url);  // find same url in graph
	Node* val = NULL;
	if(id>=0)
		val = arr_url[id];
	 
	if(val!=NULL)
	{
		my_queue* new_queue = new my_queue;
	        new_queue->set_node(i_prior);
		
		val->v_count++; 
		i_prior->insert_edge(val);  // add edge
		val->set_Prior(i_prior);
		
		val->queue->Enqueue(val->queue, new_queue); //  for log  when  end of browser 
	
		return val;
	}
	else    // new node
	{
		my_queue* new_queue = new my_queue;
		new_queue->set_node(i_prior);
		Node* New = new Node(i_url,i_prior);
		i_prior->insert_edge(New);
		arr_url[a_index]=New;
		a_index++;
		QuickSort(0,a_index-1);
		New->queue->Enqueue(New->queue, new_queue);
		
		return New;
	}
	return 0;
}

void Graph::discount(char* i_url)
{
	if(!head)
		return;
	int id = BinarySearch(a_index,i_url);

	if (id < 0) return;  

	Node* val = arr_url[id];
	if(val!=0)  
	{
		Node* New = new Node;
		val->v_count--;
		if (val != head){
			New = val->queue->Dequeue(val->queue)->get_node();
			New->delete_edge(val);
		}

		if(val->v_count==0)
		{
			memmove(arr_url+id,arr_url+id+1,(a_index-id-1)*sizeof(arr_url[0]));
			arr_url[a_index-1]=NULL;
			a_index--;
		}
	}

	return;
}

int Graph::BinarySearch(int Size,char* i_url)
{
    int Left, Right, Mid;

    Left = 0;
    Right = Size-1;

    while (Left <= Right)
    {
        Mid = (Left + Right) / 2;
	if (strcmp(i_url,arr_url[Mid]->url)==0)//Target == arr_url[Mid])
            return Mid;
        else if (strcmp(i_url,arr_url[Mid]->url)>0)
            Left = Mid + 1;
        else
            Right = Mid - 1;
    }
    return -1;
}
Node* Graph::gethead()
{
	return head;
}

//////////////////////////////////////////////start_ sorting function 
void Graph::Swap(int l, int r)
{
    Node* temp = arr_url[l];
	arr_url[l] = arr_url[r];
	arr_url[r] = temp;
}


int Graph::Partition(int Left,int Right)
{
    int First = Left;
    Node* Pivot = arr_url[First];

    ++Left;

    while( Left <= Right )
    {
		while((strcmp(arr_url[Left]->url,Pivot->url)<=0) /*DataSet[Left] <= Pivot*/ && Left < Right )
            ++Left;
        while((strcmp(arr_url[Right]->url,Pivot->url)>=0) /*DataSet[Right] >= Pivot*/ && Left <= Right )
            --Right;

        if ( Left < Right )
            Swap(Left,Right);
        else
            break;
    }

    Swap(First, Right);

    return Right;
}

void Graph::QuickSort(int Left, int Right)
{
    if ( Left < Right )
    {
        int Index = Partition(Left,Right);

        QuickSort(Left,Index - 1 );
        QuickSort(Index + 1,Right);
    }
}
/////////////////////////////////////////////////////////end sorting function

void Graph:: set_temp_node(Node* temp_n)
{
	temp_node = temp_n;
}
Node* Graph:: get_temp_node()
{
	return temp_node;
}
