#pragma once

typedef struct Brick
{
	Object object;
	Location location;
	bool raise;
	int targethp;
	ChildImage type;
}Brick;

typedef struct ChildBrick
{
	Object object;
	Location location;
	int initialvelocity;
}ChildBrick;

Brick* BrickConstructor(Game *game,ChildImage type);