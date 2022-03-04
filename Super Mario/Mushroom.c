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
#include "Mushroom.h"

bool MushroomAlterSourceRectangle(Object*);
void MushroomMove(Object*,Chain*,unsigned char[256]);
bool MushroomImpact(Object*,Object*,bool);
void * MushroomUpdateData(Object*,bool);

Mushroom * MushroomConstructor(Game * game)
{
	Mushroom *mushroom=calloc(1,sizeof(Mushroom));
	if(!ObjectConstructor((Object*)mushroom,game,ID_MUSHROOM))
	{
		free(mushroom);
		return NULL;
	}
	mushroom->object.type=type_mushroom;
	mushroom->object.priority=1;
	mushroom->object.state=shrink;
	mushroom->object.facetoleft=false;
	mushroom->object.speed=4;
	mushroom->object.AlterSourceRectangle=MushroomAlterSourceRectangle;
	mushroom->object.Move=MushroomMove;
	mushroom->object.Impact=MushroomImpact;
	mushroom->object.UpdateData=MushroomUpdateData;
	PROPVARIANT propvariant={0};
	propvariant.vt=VT_EMPTY;
	Sound *sound=&game->sound[mushroomappeared];
	sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
	sound->playing=true;
	return mushroom;
}

bool MushroomAlterSourceRectangle(Object *object)
{
	int x=0,count=3;
	switch(object->game->point)
	{
	}
	CommonAlter(object,x,count);
	return true;
}

void MushroomMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==shrink)
	{
		destination->top-=object->speed;
		destination->bottom-=object->speed;
		if(!object->Impact(object,((Mushroom*)object)->parent,true))
			object->state=stand;
	}
	else
	{
		int direction=1;
		if(object->facetoleft)
			direction=-1;
		if(!MoveIfValidate(object,chain,object->speed,direction,false))
			object->facetoleft=!object->facetoleft;
	}
}

bool MushroomImpact(Object *self,Object *others,bool IsValidate)
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
				if(self->state==stand)
				{
					self->state=dead;
					PROPVARIANT propvariant={0};
					propvariant.vt=VT_EMPTY;
					Sound *sound=&self->game->sound[eatmushroom];
					sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
					sound->playing=true;
				}
				break;
		}
	}
	return false;
}

void * MushroomUpdateData(Object *object,bool update)
{
	static Mushroom temp[2],*pointer;
	if(update)
	{
		*((Mushroom*)object)=*pointer;
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
		*pointer=*((Mushroom*)object);
		return pointer;
	}
}