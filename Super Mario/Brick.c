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
#include "Brick.h"
#include "Mario.h"

bool BrickAlterSourceRectangle(Object*);
bool BrickImpact(Object*,Object*,bool);
void BrickReaction(Object*);
void * BrickUpdateData(Object*,bool);

ChildBrick* ChildBrickConstructor(Game *game,int);
bool ChildBrickAlterSourceRectangle(Object*);
void ChildBrickMove(Object*,Chain*,unsigned char[256]);

Brick* BrickConstructor(Game *game,ChildImage type)
{
	Brick *brick=calloc(1,sizeof(Brick));
	if(!ObjectConstructor((Object*)brick,game,ID_BACKGROUND))
	{
		free(brick);
		return NULL;
	}
	brick->object.type=type_brick;
	brick->object.priority=1;
	brick->object.AlterSourceRectangle=BrickAlterSourceRectangle;
	brick->object.Gravity=NULL;
	brick->object.Impact=BrickImpact;
	brick->object.Reaction=BrickReaction;
	brick->object.UpdateData=BrickUpdateData;
	brick->location=LT;
	brick->type=type;
	return brick;
}

bool BrickAlterSourceRectangle(Object *object)
{
	int x=0,y=0;
	Brick *brick=(Brick*)object;
	switch(brick->type)
	{
		case CrackedStoneBrick:
			x=0,y=4;
			break;
		case ChiseledStoneBrick:
			x=0,y=5;
			break;
	}
	switch(object->game->point)
	{
	}
	BackgroundCommonAlter(object,brick->location,x,y);
	return true;
}

bool BrickImpact(Object *self,Object *others,bool IsValidate)
{
	Brick *brick=(Brick*)self;
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
				if(!brick->raise)
				{
					self->destination.top-=10;
					self->destination.bottom-=10;
					self->duration=10;
					brick->raise=true;
					brick->targethp=((Mario*)others)->hp;
				}
				break;
		}
	}
	return false;
}

void BrickReaction(Object *object)
{
	Brick *brick=(Brick*)object;
	if(brick->raise)
	{
		if(object->duration>0)
			object->duration--;
		if(brick->targethp==1)
		{
			if(object->duration==0)
			{
				object->destination.top+=10;
				object->destination.bottom+=10;
				brick->raise=false;
				PROPVARIANT propvariant={0};
				propvariant.vt=VT_EMPTY;
				Sound *sound=&object->game->sound[touchedbrick];
				sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
				sound->playing=true;
			}
		}
		else if(object->duration==8)
		{
			object->state=dead;
			object->duration=0;
			for(int i=0;i<4;i++)
			{
				ChildBrick *childbrick=ChildBrickConstructor(object->game,i);
				assert(childbrick);
				childbrick->object.destination=brick->object.destination;
				childbrick->object.destination.bottom-=object->game->destinationsize/2;
				switch(i)
				{
					case 0:
					case 1:
						childbrick->object.destination.left-=object->game->destinationsize/2;
						childbrick->object.destination.right-=object->game->destinationsize/2;
						break;
					case 2:
					case 3:
						childbrick->object.destination.left+=object->game->destinationsize/2;
						childbrick->object.destination.right+=object->game->destinationsize/2;
						break;
				}
				object->game->chain->add(object->game->chain,childbrick);
			}
		}
	}
}

void * BrickUpdateData(Object *object,bool update)
{
	static Brick temp[2],*pointer;
	if(update)
	{
		*((Brick*)object)=*pointer;
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
		*pointer=*((Brick*)object);
		return pointer;
	}
}

ChildBrick * ChildBrickConstructor(Game * game,int num)
{
	ChildBrick *brick=calloc(1,sizeof(ChildBrick));
	if(!ObjectConstructor((Object*)brick,game,ID_BACKGROUND))
	{
		free(brick);
		return NULL;
	}
	brick->object.type=type_ornament;
	brick->object.priority=1;
	brick->object.speed=10;
	brick->object.duration=60;
	brick->object.AlterSourceRectangle=ChildBrickAlterSourceRectangle;
	brick->object.Move=ChildBrickMove;
	brick->object.Gravity=NULL;
	brick->location=LT;
	switch(num)
	{
		case 0:
			brick->initialvelocity=15;
			brick->object.facetoleft=true;
			PROPVARIANT propvariant={0};
			propvariant.vt=VT_EMPTY;
			Sound *sound=&game->sound[breakedbrick];
			sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
			sound->playing=true;
			break;
		case 1:
			brick->initialvelocity=5;
			brick->object.facetoleft=true;
			break;
		case 2:
			brick->initialvelocity=15;
			brick->object.facetoleft=false;
			break;
		case 3:
			brick->initialvelocity=5;
			brick->object.facetoleft=false;
			break;
	}
	return brick;
}

bool ChildBrickAlterSourceRectangle(Object *object)
{
	int x=0,y=9;
	ChildBrick *brick=(ChildBrick*)object;
	switch(object->game->point)
	{
	}
	if(!object->facetoleft)
		x=1;
	BackgroundCommonAlter(object,brick->location,x,y);
	if(brick->initialvelocity==15)
		object->source.bottom-=object->game->sourcesize/2;
	else
		object->source.top+=object->game->sourcesize/2;
	return true;
}

void ChildBrickMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	ChildBrick *brick=(ChildBrick*)object;
	if(object->duration==0)
		object->state=dead;
	else
	{
		object->duration--;
		int coefficient;
		if(object->facetoleft)
			coefficient=-1;
		else
			coefficient=1;
		object->destination.left+=object->speed*coefficient;
		object->destination.right+=object->speed*coefficient;
		int speed;
		speed=brick->initialvelocity-object->game->acceleration*++object->falltime;
		object->destination.top-=speed;
		object->destination.bottom-=speed;
	}
}