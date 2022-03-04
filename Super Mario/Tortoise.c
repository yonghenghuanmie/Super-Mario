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
#include "Tortoise.h"

bool TortoiseAlterSourceRectangle(Object*);
void TortoiseMove(Object*,Chain*,unsigned char[256]);
bool TortoiseImpact(Object*,Object*,bool);
void TortoiseReaction(Object*);
void * TortoiseUpdateData(Object*,bool);

Tortoise * TortoiseConstructor(Game * game,bool isfly)
{
	Tortoise *tortoise=calloc(1,sizeof(Tortoise));
	if(!ObjectConstructor((Object*)tortoise,game,ID_TORTOISE))
	{
		free(tortoise);
		return NULL;
	}
	tortoise->object.type=type_tortoise;
	tortoise->object.priority=2;
	tortoise->object.facetoleft=true;
	tortoise->object.speed=3;
	tortoise->object.AlterSourceRectangle=TortoiseAlterSourceRectangle;
	tortoise->object.Move=TortoiseMove;
	tortoise->object.Impact=TortoiseImpact;
	tortoise->object.Reaction=TortoiseReaction;
	tortoise->object.UpdateData=TortoiseUpdateData;
	tortoise->isfly=isfly;
	tortoise->grade=200;
	return tortoise;
}

bool TortoiseAlterSourceRectangle(Object *object)
{
	int x,count=10;
	switch(object->state)
	{
		case stand:
			if(object->facetoleft)
				x=object->duration/10+2;
			else
				x=object->duration/10+4;
			break;

		case shrink:
			x=8;
			if(!((Tortoise*)object)->moving&&object->duration<20)
				break;
		case dead:
			x=9;
			break;
	}
	CommonAlter(object,x,count);
	return true;
}

void TortoiseMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	Tortoise *tortoise=(Tortoise*)object;
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==stand||(object->state==shrink&&((Tortoise*)object)->moving))
	{
		int direction=1;
		if(object->facetoleft)
			direction=-1;
		if(object->state==shrink&&((Tortoise*)object)->moving)
			direction*=2;
		destination->left+=object->speed*direction;
		destination->right+=object->speed*direction;
		for(Node *node=chain->node;node;node=node->next)
		{
			Object *temp=node->data;
			if(temp==object)
				continue;
			if((!(object->state==shrink&&tortoise->moving)&&temp->type==type_mushroommonster)||
				(!(object->state==shrink&&tortoise->moving)&&temp->type==type_tortoise)||
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
}

bool TortoiseImpact(Object *self,Object *others,bool IsValidate)
{
	Tortoise *tortoise=(Tortoise*)self;
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
				if(others->state==float_)
				{
					if(self->state==stand)
					{
						self->state=shrink;
						self->duration=300;
					}
					else if(self->state==shrink&&!tortoise->moving)
					{
						if(others->destination.left<=self->destination.left)
							self->facetoleft=false;
						else
							self->facetoleft=true;
						float movepixel;
						if(self->facetoleft)
						{
							movepixel=self->destination.right-others->destination.left;
							self->destination.left-=movepixel;
							self->destination.right-=movepixel;
						}
						else
						{
							movepixel=others->destination.right-self->destination.left;
							self->destination.left+=movepixel;
							self->destination.right+=movepixel;
						}
						self->duration=0;
						tortoise->moving=true;
					}
				}
				else if(self->state==shrink&&!tortoise->moving)
				{
					self->state=dead;
					self->duration=100;
					self->game->totalgrade+=tortoise->grade;
				}
				break;

			case type_tortoise:
				if(self->state==stand)
					if(others->state==shrink&&((Tortoise*)others)->moving)
					{
						self->state=dead;
						self->duration=100;
						self->game->totalgrade+=tortoise->grade;
					}
				break;

			case type_bullet:
			{
				self->state=dead;
				self->duration=100;
				self->game->totalgrade+=tortoise->grade;
				PROPVARIANT propvariant={0};
				propvariant.vt=VT_EMPTY;
				Sound *sound=&self->game->sound[killedbybullet];
				sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
				sound->playing=true;
			}
			break;

			case type_box:
			case type_brick:
			{
				self->state=dead;
				self->duration=100;
				self->game->totalgrade+=tortoise->grade;
			}
			break;
		}
	}
	return false;
}

void TortoiseReaction(Object *object)
{
	switch(object->state)
	{
		case stand:
			object->duration++;
			object->duration%=20;
			break;

		case shrink:
			if(!((Tortoise*)object)->moving&&object->duration==0)
				object->state=stand;
			if(object->duration>0)
				object->duration--;
			break;

		case dead:
			object->destination.top+=10;
			object->destination.bottom+=10;
			if(object->duration>0)
				object->duration--;
			break;
	}
}

void * TortoiseUpdateData(Object *object,bool update)
{
	static Tortoise temp[2],*pointer;
	if(update)
	{
		*((Tortoise*)object)=*pointer;
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
		*pointer=*((Tortoise*)object);
		return pointer;
	}
}