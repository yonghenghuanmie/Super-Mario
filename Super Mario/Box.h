#pragma once

typedef struct Box
{
	Object object;
	Location location;
	TYPE item;
	bool raise;
}Box;

Box* BoxConstructor(Game *game,TYPE item);