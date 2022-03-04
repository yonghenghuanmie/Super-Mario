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
#include "Mario.h"
#include "Box.h"
#include "FlashCoin.h"
#include "Mushroom.h"
#include "Flower.h"
#include "Star.h"

bool BoxAlterSourceRectangle(Object*);
bool BoxImpact(Object*,Object*,bool);
void BoxReaction(Object*);
void* BoxUpdateData(Object*,bool);

Box* BoxConstructor(Game *game,TYPE item)
{
	Box *box=calloc(1,sizeof(Box));
	if(!ObjectConstructor((Object*)box,game,ID_BACKGROUND))
	{
		free(box);
		return NULL;
	}
	box->object.type=type_box;
	box->object.priority=2;
	box->object.AlterSourceRectangle=BoxAlterSourceRectangle;
	box->object.Gravity=NULL;
	box->object.Impact=BoxImpact;
	box->object.Reaction=BoxReaction;
	box->object.UpdateData=BoxUpdateData;
	box->location=LB;
	box->item=item;
	box->raise=false;
	return box;
}

bool BoxAlterSourceRectangle(Object *object)
{
	int y=0;
	Box *box=(Box*)object;
	if(object->state==stand)
		switch(object->game->point)
		{
		}
	else
		y=1;
	BackgroundCommonAlter(object,box->location,object->duration/20,y);
	return true;
}

bool BoxImpact(Object *self,Object *others,bool IsValidate)
{
	Box *box=(Box*)self;
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
					self->state=opened;
					box->location=LT;
				}
				if(!box->raise)
				{
					self->duration=0;
					self->destination.top-=10;
					self->destination.bottom-=10;
					box->raise=true;
				}
				break;
		}
	}
	return false;
}

void BoxReaction(Object *object)
{
	Box *box=(Box*)object;
	if(box->raise)
	{
		object->duration++;
		if(object->duration==10)
		{
			object->destination.top+=10;
			object->destination.bottom+=10;
			box->raise=false;
			object->duration=0;
		}
	}
	switch(object->state)
	{
		case stand:
			object->duration++;
			object->duration%=60;
			break;

		case opened:
		{
			Object *item=NULL;
			switch(((Box*)object)->item)
			{
				case 0:
					for(Node *node=object->game->chain->node;node;node=node->next)
					{
						Object *temp=node->data;
						if(temp->type==type_mario)
						{
							Mario *mario=(Mario*)temp;
							if(mario->hp==1)
							{
								item=(Object*)MushroomConstructor(object->game);
								assert(item);
								((Mushroom*)item)->parent=object;
							}
							else if(mario->hp==2)
							{
								item=(Object*)FlowerConstructor(object->game);
								assert(item);
								((Flower*)item)->parent=object;
							}
							break;
						}
					}
					break;

				case type_coin:
					item=(Object*)FlashCoinConstructor(object->game);
					assert(item);
					object->game->totalgrade+=((FlashCoin*)item)->grade;
					break;
				case type_star:
					item=(Object*)StarConstructor(object->game);
					assert(item);
					((Star*)item)->parent=object;
					break;
			}
			if(item)
			{
				item->destination=object->destination;
				if(((Box*)object)->item==type_coin)
				{
					item->destination.left+=object->game->destinationsize/4;
					item->destination.right-=object->game->destinationsize/4;
				}
				item->game->chain->add(item->game->chain,item);
				object->state=invalidate;
			}
		}
		break;
	}
}

void * BoxUpdateData(Object *object,bool update)
{
	static Box temp[2],*pointer;
	if(update)
	{
		*((Box*)object)=*pointer;
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
		*pointer=*((Box*)object);
		return pointer;
	}
}