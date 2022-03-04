#pragma once

typedef struct Star
{
	Object object;
	Location location;
	int initialvelocity;
	Object *parent;
}Star;

Star * StarConstructor(Game * game);