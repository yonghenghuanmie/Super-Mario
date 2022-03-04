#pragma once

typedef struct Mario
{
	Object object;
	int hp;
	int invincible;
	bool weapon;
	//const
	//int jumpframe;
	//double basespeed;
	int initialvelocity;
	STATE laststate;
	int lastduration;
	int reload;
	bool animate;
}Mario;

Mario* MarioConstructor(Game *game);