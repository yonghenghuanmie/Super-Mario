#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <tchar.h>
#include <d2d1.h>
#include <wincodec.h>
#include <mfapi.h>
#include <mfidl.h>
#include "Draw.h"
#include "Sound.h"
#include "Chain.h"
#include "Map.h"
#include "Object.h"
#include "Background.h"

bool BackgroundAlterSourceRectangle(Object*);
void BackgroundReaction(Object*);

Background* BackgroundConstructor(Game *game)
{
	Background *background=calloc(1,sizeof(Background));
	if(!ObjectConstructor((Object*)background,game,ID_BACKGROUND))
	{
		free(background);
		return NULL;
	}
	background->object.type=type_background;
	background->object.AlterSourceRectangle=BackgroundAlterSourceRectangle;
	background->object.Gravity=NULL;
	background->object.Reaction=BackgroundReaction;
	background->location=LB;
	return background;
}

bool BackgroundAlterSourceRectangle(Object *object)
{
	int x=0,y=8;
	Background *background=(Background*)object;
	switch(object->game->point)
	{
	}
	BackgroundCommonAlter(object,background->location,x,y);
	return true;
}

void BackgroundReaction(Object *object)
{
	Sound *sound=&object->game->sound[backgroundmusic];
	if(!sound->playing)
	{
		PROPVARIANT propvariant={0};
		propvariant.vt=VT_EMPTY;
		sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
		sound->playing=true;
	}
}
