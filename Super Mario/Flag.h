#pragma once

typedef struct Flag
{
	Object object;
	Location location;
	ChildImage id;
	void (*Destructor)(struct Object*);
}Flag;

Flag * FlagConstructor(Game * game,ChildImage id);