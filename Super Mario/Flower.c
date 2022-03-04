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
#include "Flower.h"

bool FlowerAlterSourceRectangle(Object*);
void FlowerMove(Object*,Chain*,unsigned char[256]);
bool FlowerImpact(Object*,Object*,bool);
void* FlowerUpdateData(Object*,bool);

Flower * FlowerConstructor(Game * game)
{
	Flower *flower=calloc(1,sizeof(Flower));
	if(!ObjectConstructor((Object*)flower,game,ID_FLOWERANDSTAR))
	{
		free(flower);
		return NULL;
	}
	flower->object.type=type_flower;
	flower->object.priority=1;
	flower->object.state=shrink;
	flower->object.speed=4;
	flower->object.AlterSourceRectangle=FlowerAlterSourceRectangle;
	flower->object.Move=FlowerMove;
	flower->object.Gravity=NULL;
	flower->object.Impact=FlowerImpact;
	flower->object.UpdateData=FlowerUpdateData;
	flower->location=LT;
	PROPVARIANT propvariant={0};
	propvariant.vt=VT_EMPTY;
	Sound *sound=&game->sound[mushroomappeared];
	sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
	sound->playing=true;
	return flower;
}

bool FlowerAlterSourceRectangle(Object *object)
{
	int x=0,y=0;
	Flower *flower=(Flower*)object;
	switch(object->game->point)
	{
	}
	BackgroundCommonAlter(object,flower->location,x,y);
	return true;
}

void FlowerMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==shrink)
	{
		destination->top-=object->speed;
		destination->bottom-=object->speed;
		if(!object->Impact(object,((Flower*)object)->parent,true))
			object->state=stand;
	}
}

bool FlowerImpact(Object *self,Object *others,bool IsValidate)
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

void * FlowerUpdateData(Object *object,bool update)
{
	static Flower temp[2],*pointer;
	if(update)
	{
		*((Flower*)object)=*pointer;
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
		*pointer=*((Flower*)object);
		return pointer;
	}
}
