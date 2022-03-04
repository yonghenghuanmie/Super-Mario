#pragma once

typedef struct Coin
{
	Object object;
	Location location;
	int grade;
}Coin;

Coin* CoinConstructor(Game *game);