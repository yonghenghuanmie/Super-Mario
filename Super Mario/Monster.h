#pragma once

typedef struct MushroomMonster
{
	Movable movable;

	bool (*AlterSourceRectangle)(Image*);
}MushroomMonster;

MushroomMonster* MushroomMonsterConstructor(Draw*);

typedef struct Tortoise
{
	Movable movable;

	bool (*AlterSourceRectangle)(Image*);
}Tortoise;