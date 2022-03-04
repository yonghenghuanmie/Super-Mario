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
#include "MushroomMonster.h"
#include "Tortoise.h"

bool MushroomMonsterAlterSourceRectangle(Object*);
void MushroomMonsterMove(Object*,Chain*,unsigned char[256]);
bool MushroomMonsterImpact(Object*,Object*,bool);
void MushroomMonsterReaction(Object*);
void * MushroomMonsterUpdateData(Object*,bool);

MushroomMonster* MushroomMonsterConstructor(Game *game)
{
	MushroomMonster *mushroommonster=calloc(1,sizeof(MushroomMonster));
	if(!ObjectConstructor((Object*)mushroommonster,game,ID_MUSHROOMMONSTER))
	{
		free(mushroommonster);
		return NULL;
	}
	mushroommonster->object.type=type_mushroommonster;
	mushroommonster->object.priority=2;
	mushroommonster->object.facetoleft=true;
	mushroommonster->object.speed=3;
	mushroommonster->object.AlterSourceRectangle=MushroomMonsterAlterSourceRectangle;
	mushroommonster->object.Move=MushroomMonsterMove;
	mushroommonster->object.Impact=MushroomMonsterImpact;
	mushroommonster->object.Reaction=MushroomMonsterReaction;
	mushroommonster->object.UpdateData=MushroomMonsterUpdateData;
	mushroommonster->fall=false;
	mushroommonster->grade=100;
	return mushroommonster;
}

bool MushroomMonsterAlterSourceRectangle(Object *object)
{
	int x=0,count=4;
	MushroomMonster *mushroommonster=(MushroomMonster*)object;
	switch(mushroommonster->object.state)
	{
		case stand:
			if(mushroommonster->object.facetoleft)
				x=1;
			break;

		case dead:
			if(mushroommonster->fall)
				x=3;
			else
				x=2;
			break;
	}
	CommonAlter(object,x,count);
	return true;
}

void MushroomMonsterMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	int direction=1;
	if(object->facetoleft)
		direction=-1;
	destination->left+=object->speed*direction;
	destination->right+=object->speed*direction;
	for(Node *node=chain->node;node;node=node->next)
	{
		Object *temp=node->data;
		if(temp==object)
			continue;
		if(temp->type==type_mushroommonster||
			temp->type==type_tortoise||
			temp->type==type_box||
			temp->type==type_brick||
			temp->type==type_pipe)
			if(object->Impact(object,temp,true))
			{
				destination->left-=object->speed*direction;
				destination->right-=object->speed*direction;
				object->facetoleft=!object->facetoleft;
				break;
			}
	}
}

bool MushroomMonsterImpact(Object *self,Object *others,bool IsValidate)
{
	MushroomMonster *mushroommonster=(MushroomMonster*)self;
	D2D1_RECT_F *rect_first=&self->destination,
		*rect_next=&others->destination;
	if(rect_first->bottom>rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top<rect_next->bottom&&rect_first->left<rect_next->right)
	{
		if(IsValidate)
			return true;
		PROPVARIANT propvariant={0};
		propvariant.vt=VT_EMPTY;
		Sound *sound;
		switch(others->type)
		{
			case type_mario:
				if(others->state==float_)
				{
					self->state=dead;
					self->duration=15;
					self->game->totalgrade+=mushroommonster->grade;
					sound=&self->game->sound[mushroommonstertrampled];
					sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
					sound->playing=true;
				}
				break;

			case type_tortoise:
				if(others->state==shrink&&((Tortoise*)others)->moving)
				{
					self->state=dead;
					self->duration=100;
					mushroommonster->fall=true;
					self->game->totalgrade+=mushroommonster->grade;
				}
				break;

			case type_bullet:
			{
				self->state=dead;
				self->duration=100;
				mushroommonster->fall=true;
				self->game->totalgrade+=mushroommonster->grade;
				sound=&self->game->sound[killedbybullet];
				sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
				sound->playing=true;
			}
			break;

			case type_box:
			case type_brick:
			{
				self->state=dead;
				self->duration=100;
				mushroommonster->fall=true;
				self->game->totalgrade+=mushroommonster->grade;
			}
			break;
		}
	}
	return false;
}

void MushroomMonsterReaction(Object *object)
{
	if(object->state==dead)
	{
		if(((MushroomMonster*)object)->fall)
		{
			object->destination.top+=10;
			object->destination.bottom+=10;
		}
		if(object->duration>0)
			object->duration--;
	}
}

void * MushroomMonsterUpdateData(Object *object,bool update)
{
	static MushroomMonster temp[2],*pointer;
	if(update)
	{
		*((MushroomMonster*)object)=*pointer;
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
		*pointer=*((MushroomMonster*)object);
		return pointer;
	}
}