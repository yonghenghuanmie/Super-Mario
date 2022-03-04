#include <stdlib.h>
#include <stdbool.h>
#include "Chain.h"

void ChainDestructor(Chain*);
bool add(Chain*,void*);
Node* remove(Chain*,Node*);
Node* ChainNext(Node*);
void* ChainGetData(Node*);

Chain* ChainConstructor()
{
	Chain *chain=calloc(1,sizeof(Chain));
	if(chain)
	{
		chain->Destructor=ChainDestructor;
		chain->add=add;
		chain->remove=remove;
		chain->node=NULL;
	}
	return chain;
}

void ChainDestructor(Chain *chain)
{
	for(Node *node=chain->node;node!=NULL;)
	{
		Node *temp=node->next;
		free(node);
		node=temp;
	}
	free(chain);
}

bool add(Chain *chain,void *data)
{
	Node *node=calloc(1,sizeof(Node));
	if(node==NULL)
		return false;
	node->data=data;
	node->next=NULL;
	if(chain->node==NULL)
		chain->node=node;
	else
	{
		Node *temp;
		for(temp=chain->node;temp->next!=NULL;temp=temp->next);
		temp->next=node;
	}
	return true;
}

Node* remove(Chain *chain,Node *NodePointer)
{
	for(Node *node=chain->node,*previous=NULL;node!=NULL;previous=node,node=node->next)
		if(node==NodePointer)
		{
			Node *Next=node->next;
			if(previous==NULL)
				chain->node=node->next;
			else
				previous->next=node->next;
			free(node);
			return Next;
		}
	return NULL;
}