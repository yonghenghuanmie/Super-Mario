#pragma once

typedef struct FlashCoin
{
	Object object;
	int grade;
}FlashCoin;

FlashCoin* FlashCoinConstructor(Game *game);