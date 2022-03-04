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
#include "Castle.h"

void CastleDestructor(Object*);
bool CastleAlterSourceRectangle(Object*);
void CastleMove(Object*,Chain*,unsigned char[256]);
bool CastleImpact(Object*,Object*,bool);
void * CastleUpdateData(Object*,bool);
bool upper;

Castle * CastleConstructor(Game * game,ChildImage id)
{
	Castle *castle=calloc(1,sizeof(Castle));
	if(!ObjectConstructor((Object*)castle,game,ID_BACKGROUND))
	{
		free(castle);
		return NULL;
	}
	castle->object.type=type_castle;
	castle->object.priority=2;
	castle->object.state=stand;
	castle->object.speed=3;
	castle->object.AlterSourceRectangle=CastleAlterSourceRectangle;
	castle->object.Move=CastleMove;
	castle->object.Gravity=NULL;
	castle->object.Impact=CastleImpact;
	castle->object.UpdateData=CastleUpdateData;
	castle->object.Gravity=NULL;
		castle->location=RB;
	castle->id=id;
	castle->parent=NULL;
	castle->Destructor=castle->object.Destructor;
	castle->object.Destructor=CastleDestructor;
	return castle;
}

void CastleDestructor(Object *object)
{
	upper=false;
	((Castle*)object)->Destructor(object);
}

bool CastleAlterSourceRectangle(Object *object)
{
	int x,y;
	Castle *castle=(Castle*)object;
	if(castle->id>=castle1_1&&castle->id<=castle2_25)
	{
		ChildImage id;
		switch(object->game->point)
		{
			case 1:
				id=castle1_1;
				x=0,y=5;
				break;
		}
		x+=(castle->id-id)/5;
		y+=(castle->id-id)%5;
	}
	else if(castle->id==castleflag)
		x=8,y=4;
	else assert(0);
	BackgroundCommonAlter(object,castle->location,x,y);
	return true;
}

void CastleMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==jump)
	{
		destination->top-=object->speed;
		destination->bottom-=object->speed;
		if(!object->Impact(object,((Castle*)object)->parent,true))
		{
			object->state=stand;
			object->game->newstage=true;
		}
	}
}

bool CastleImpact(Object *self,Object *others,bool IsValidate)
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
				if(!upper)
				{
					Object *object=(Object*)CastleConstructor(self->game,castleflag);
					assert(object);
					object->priority=1;
					object->state=jump;
					for(Node *node=self->game->chain->node;node;node=node->next)
					{
						Object *temp=(Object*)node->data;
						if(temp->type==type_castle&&(((Castle*)temp)->id==castle1_11||((Castle*)temp)->id==castle2_11))
						{
							((Castle*)object)->parent=temp;
							object->destination=temp->destination;
							break;
						}
					}
					object->game->chain->add(object->game->chain,object);
					upper=true;
				}
				break;
		}
	}
	return false;
}

void * CastleUpdateData(Object *object,bool update)
{
	static Castle temp[2],*pointer;
	if(update)
	{
		*((Castle*)object)=*pointer;
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
		*pointer=*((Castle*)object);
		return pointer;
	}
}
