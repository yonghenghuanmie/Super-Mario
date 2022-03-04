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
#include "Flag.h"

void FlagDestructor(Object*);
bool FlagAlterSourceRectangle(Object*);
void FlagMove(Object*,Chain*,unsigned char[256]);
bool FlagImpact(Object*,Object*,bool);
void * FlagUpdateData(Object*,bool);
bool lower;

Flag * FlagConstructor(Game * game,ChildImage id)
{
	Flag *flag=calloc(1,sizeof(Flag));
	if(!ObjectConstructor((Object*)flag,game,ID_BACKGROUND))
	{
		free(flag);
		return NULL;
	}
	flag->object.type=type_flag;
	flag->object.priority=1;
	flag->object.state=stand;
	flag->object.speed=10;
	flag->Destructor=flag->object.Destructor;
	flag->object.Destructor=FlagDestructor;
	flag->object.AlterSourceRectangle=FlagAlterSourceRectangle;
	flag->object.Move=FlagMove;
	flag->object.Gravity=NULL;
	flag->object.Impact=FlagImpact;
	flag->object.UpdateData=FlagUpdateData;
	flag->location=RB;
	flag->id=id;
	return flag;
}

void FlagDestructor(Object *object)
{
	lower=false;
	((Flag*)object)->Destructor(object);
}

bool FlagAlterSourceRectangle(Object *object)
{
	int x,y;
	ChildImage id;
	Flag *flag=(Flag*)object;
	switch(object->game->point)
	{
		case 1:
			id=flag1_1;
			x=3;
			break;
	}
	switch(flag->id-id)
	{
		case 0:
			y=1;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			x+=1;
			y=flag->id-id-1;
			break;
	}
	BackgroundCommonAlter(object,flag->location,x,y);
	return true;
}

void FlagMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	Flag *flag=(Flag*)object;
	if(object->state==float_)
	{
		object->destination.top+=object->speed;
		object->destination.bottom+=object->speed;
	}
}

bool FlagImpact(Object *self,Object *others,bool IsValidate)
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
				if(!lower)
					for(Node *node=self->game->chain->node;node;node=node->next)
					{
						Object *object=(Object*)node->data;
						if(object->type==type_flag)
						{
							PROPVARIANT propvariant={0};
							propvariant.vt=VT_EMPTY;
							Sound *sound=&object->game->sound[lowerflag];
							sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
							sound->playing=true;

							Flag *flag=(Flag*)object;
							ChildImage id;
							switch(object->game->point)
							{
								case 1:
									id=flag1_1;
									break;
							}
							if(flag->id==id||flag->id==id+2)
							{
								if(flag->id==id+2)
								{
									Object *temp=(Object*)FlagConstructor(object->game,id+3);
									assert(temp);
									temp->destination=object->destination;
									temp->game->chain->add(temp->game->chain,temp);
								}
								object->state=float_;
								lower=true;
							}
						}
					}
				break;

			case type_brick:
				self->state=dead;
				break;
		}
	}
	return false;
}

void * FlagUpdateData(Object *object,bool update)
{
	static Flag temp[2],*pointer;
	if(update)
	{
		*((Flag*)object)=*pointer;
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
		*pointer=*((Flag*)object);
		return pointer;
	}
}
