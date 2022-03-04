#pragma once

typedef struct Node
{
	void *data;
	struct Node *next;
}
Node;

typedef struct Chain
{
	void (*Destructor)(struct Chain*);
	bool (*add)(struct Chain*,void*);
	Node* (*remove)(struct Chain*,Node*);

	Node *node;
}Chain;

Chain* ChainConstructor();