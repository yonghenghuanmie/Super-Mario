#pragma once

typedef struct Ornament
{
	Object object;
	Location location;
	ChildImage id;
}Ornament;

Ornament* OrnamentConstructor(Game *game,ChildImage id);