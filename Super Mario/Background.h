#pragma once

typedef struct Background
{
	Object object;
	Location location;
}Background;

Background* BackgroundConstructor(Game *game);