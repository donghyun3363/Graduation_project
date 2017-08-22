#include "Node.h"

Node::Node(void)
{
    url = NULL;
	v_count =0;
	memset(e_count,0,50);
	memset(edge,NULL,50);
	memset(Prior,NULL,50);
	e_index = 0;
	p_index = 0;
	queue = new linkedqueue;
}
Node::~Node(void)
{
} 

Node::Node(char* i_url,Node* pre)
{
	url = new char[strlen(i_url)+1];
	strcpy(url,i_url);
	v_count = 1;
	memset(e_count,0,50);
	memset(edge,NULL,50);
	memset(Prior,NULL,50);
	e_index = 0;
	p_index = 1;
	queue = new linkedqueue;
	Prior[0]=pre;
}
Node** Node:: get_edge(){
	return edge;
}
int Node::get_e_index()
{
	return e_index;
}
void Node::insert_edge(Node* New)
{
	int temp=0;
	int flag =0;
	Node* temp_node;
	if (e_index > 0)
	{
		for (int i = 0; i < e_index; i++) 
		{
			if (strcmp(edge[i]->url, New->url) == 0) 
			{
				e_count[i]++;
				//return;
				flag=1;
				break;
			}
		}
	}
	if(flag == 0){
		edge[e_index]=New;
		e_count[e_index]=1;
		e_index++;
	}
	for (int i = 0; i < e_index; i++) 
	{
		for(int j=i+1;j<e_index;j++)
		{
			if(e_count[i]< e_count[j])
			{
				temp = e_count[i];
				e_count[i]=e_count[j];
				e_count[j]= temp;

				temp_node = edge[i];
				edge[i]=edge[j];
				edge[j]= temp_node;	
			}
		}	
	}
	return;
}

void Node::delete_edge(Node* New)
{
	int index=0;

	int i=0;
	while (edge[i]!=NULL)
	{
		if(strcmp(edge[i]->url, New->url)==0) 
		{
			
			e_count[i]--;
			if (e_count[i] == 0)
			{
				memmove(edge + i, edge + i + 1, (50 - i - 1)*sizeof(edge[0]));
				memmove(e_count + i, e_count + i + 1, (50 - i - 1)*sizeof(e_count[0]));
				edge[50 - 1] = NULL;
				e_count[50 - 1] = NULL;
			}	
		}
		i++;
	}
	e_index--;

	return;
}
void Node::set_Prior(Node* pre)
{
	bool valid=false;
	for(int i=1;i<p_index;i++)
	{
		if(strcmp(Prior[i]->url,pre->url)!=0)
		{
			valid = true;
			break;
		}
	}
	if(valid)
	{
		Prior[p_index]=pre;
		p_index++;
	}
	return;
}


int Node::get_p_index()
{
	return p_index;
}
Node* Node::get_Prior(int index)
{
	Node* back = Prior[index];
	Prior[index] = NULL;
	return back;
}
