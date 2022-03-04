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
#include "FlashCoin.h"

bool FlashCoinAlterSourceRectangle(Object*);
void FlashCoinMove(Object*,Chain*,unsigned char[256]);

FlashCoin * FlashCoinConstructor(Game * game)
{
	FlashCoin *coin=calloc(1,sizeof(FlashCoin));
	if(!ObjectConstructor((Object*)coin,game,ID_FLASHCOIN))
	{
		free(coin);
		return NULL;
	}
	coin->object.type=type_ornament;
	coin->object.priority=1;
	coin->object.duration=0;
	coin->object.AlterSourceRectangle=FlashCoinAlterSourceRectangle;
	coin->object.Move=FlashCoinMove;
	coin->object.Gravity=NULL;
	coin->grade=200;
	PROPVARIANT propvariant={0};
	propvariant.vt=VT_EMPTY;
	Sound *sound=&game->sound[eatcoin];
	sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
	sound->playing=true;
	return coin;
}

bool FlashCoinAlterSourceRectangle(Object *object)
{
	int count=4;
	CommonAlter(object,object->duration/5,count);
	return true;
}

void FlashCoinMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	if(object->duration<=20)
	{
		object->duration++;
		object->destination.top-=10;
		object->destination.bottom-=10;
	}
	else
	{
		object->state=dead;
		object->duration=0;
	}
}