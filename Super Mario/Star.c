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
#include "Star.h"

bool StarAlterSourceRectangle(Object*);
void StarMove(Object*,Chain*,unsigned char[256]);
void StarGravity(Object*,Chain*);
bool StarImpact(Object*,Object*,bool);
void* StarUpdateData(Object*,bool);

Star * StarConstructor(Game * game)
{
	Star *star=calloc(1,sizeof(Star));
	if(!ObjectConstructor((Object*)star,game,ID_FLOWERANDSTAR))
	{
		free(star);
		return NULL;
	}
	star->object.type=type_star;
	star->object.priority=1;
	star->object.state=shrink;
	star->object.facetoleft=false;
	star->object.speed=4;
	star->object.AlterSourceRectangle=StarAlterSourceRectangle;
	star->object.Move=StarMove;
	star->object.Gravity=StarGravity;
	star->object.Impact=StarImpact;
	star->object.UpdateData=StarUpdateData;
	star->initialvelocity=0;
	return star;
}

bool StarAlterSourceRectangle(Object *object)
{
	int x=0,y=2;
	Star *star=(Star*)object;
	switch(object->game->point)
	{
	}
	BackgroundCommonAlter(object,star->location,x,y);
	return true;
}

void StarMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==shrink)
	{
		destination->top-=object->speed;
		destination->bottom-=object->speed;
		if(!object->Impact(object,((Star*)object)->parent,true))
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

void StarGravity(Object *object,Chain *chain)
{
	Star* star=(Star*)object;
	if(MoveIfValidate(object,chain,object->game->acceleration*object->falltime-star->initialvelocity,1,true))
		object->falltime++;
	else
	{
		object->falltime=1;
		if(object->game->acceleration*object->falltime-star->initialvelocity>0)
			star->initialvelocity=25;
		else
			star->initialvelocity=0;
	}
}

bool StarImpact(Object *self,Object *others,bool IsValidate)
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
					self->state=dead;
				break;
		}
	}
	return false;
}

void * StarUpdateData(Object *object,bool update)
{
	static Star temp[2],*pointer;
	if(update)
	{
		*((Star*)object)=*pointer;
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
		*pointer=*((Star*)object);
		return pointer;
	}
}
