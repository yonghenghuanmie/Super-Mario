#pragma once

typedef struct Bullet
{
	Object object;
	int initialvelocity;
}Bullet;

Bullet * BulletConstructor(Game * game,bool facetoleft);