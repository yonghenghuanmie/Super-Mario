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
#include "Coin.h"

bool CoinAlterSourceRectangle(Object*);
bool CoinImpact(Object*,Object*,bool);
void CoinReaction(Object*);
void * CoinUpdateData(Object*,bool);

Coin * CoinConstructor(Game * game)
{
	Coin *coin=calloc(1,sizeof(Coin));
	if(!ObjectConstructor((Object*)coin,game,ID_BACKGROUND))
	{
		free(coin);
		return NULL;
	}
	coin->object.type=type_coin;
	coin->object.priority=2;
	coin->object.AlterSourceRectangle=CoinAlterSourceRectangle;
	coin->object.Gravity=NULL;
	coin->object.Impact=CoinImpact;
	coin->object.Reaction=CoinReaction;
	coin->object.UpdateData=CoinUpdateData;
	coin->location=LB;
	coin->grade=200;
	return coin;
}

bool CoinAlterSourceRectangle(Object *object)
{
	int y=2;
	Coin *coin=(Coin*)object;
	switch(object->game->point)
	{
	}
	BackgroundCommonAlter(object,coin->location,object->duration/20,y);
	return true;
}

bool CoinImpact(Object *self,Object *others,bool IsValidate)
{
	D2D1_RECT_F *rect_first=&self->destination,
		*rect_next=&others->destination;
	if(rect_first->bottom>rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top<rect_next->bottom&&rect_first->left<rect_next->right)
	{
		if(IsValidate)
			return true;
		switch(others->type)
		{
			case type_mario:
			case type_box:
			case type_brick:
				self->state=dead;
				self->duration=0;
				self->game->totalgrade+=((Coin*)self)->grade;
				PROPVARIANT propvariant={0};
				propvariant.vt=VT_EMPTY;
				Sound *sound=&self->game->sound[eatcoin];
				sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
				sound->playing=true;
				break;
		}
	}
	return false;
}

void CoinReaction(Object *object)
{
	if(object->state==stand)
	{
		object->duration++;
		object->duration%=60;
	}
}

void * CoinUpdateData(Object *object,bool update)
{
	static Coin temp[2],*pointer;
	if(update)
	{
		*((Coin*)object)=*pointer;
		if(pointer==&temp[0])
			pointer=NULL;
		else
			pointer--;
		return NULL;
	}
	else
	{
		if(pointer==NULL)
			pointer=&temp[0];
		else
			pointer++;
		*pointer=*((Coin*)object);
		return pointer;
	}
}
