#pragma once

typedef struct Flower
{
	Object object;
	Location location;
	Object *parent;
}Flower;

Flower* FlowerConstructor(Game *game);