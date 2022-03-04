#pragma once

typedef struct Tortoise
{
	Object object;
	bool isfly;
	bool moving;
	int grade;
}Tortoise;

Tortoise* TortoiseConstructor(Game *game,bool isfly);