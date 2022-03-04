#pragma once

typedef struct MushroomMonster
{
	Object object;
	bool fall;
	int grade;

	void (*Destructor)(struct Object*);
}MushroomMonster;

MushroomMonster* MushroomMonsterConstructor(Game *game);