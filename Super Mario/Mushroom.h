#pragma once

typedef struct Mushroom
{
	Object object;
	Object *parent;
}Mushroom;

Mushroom* MushroomConstructor(Game *game);