#pragma once

typedef struct Castle
{
	Object object;
	Location location;
	ChildImage id;
	Object *parent;

	void (*Destructor)(struct Object*);
}Castle;

Castle * CastleConstructor(Game * game,ChildImage id);