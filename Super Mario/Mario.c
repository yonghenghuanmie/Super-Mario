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
#include "Tortoise.h"
#include "Bullet.h"

bool MarioAlterSourceRectangle(Object*);
void MarioMove(Object*,Chain*,unsigned char[256]);
bool MarioImpact(Object*,Object*,bool);
void MarioGravity(Object*,Chain*);
void MarioReaction(Object*);
void * MarioUpdateData(Object*,bool);

Mario* MarioConstructor(Game *game)
{
	Mario *mario=calloc(1,sizeof(Mario));
	if(!ObjectConstructor((Object*)mario,game,ID_SMALLRIGHTMARIO))
	{
		free(mario);
		return NULL;
	}
	mario->object.type=type_mario;
	mario->object.priority=2;
	mario->object.facetoleft=false;
	mario->object.speed=5;
	mario->object.AlterSourceRectangle=MarioAlterSourceRectangle;
	mario->object.Move=MarioMove;
	mario->object.Impact=MarioImpact;
	mario->object.Gravity=MarioGravity;
	mario->object.Reaction=MarioReaction;
	mario->object.UpdateData=MarioUpdateData;
	mario->hp=1;
	mario->invincible=false;
	mario->weapon=false;
	//不同分辨率需要注意初速度问题
	int t=32/game->acceleration;
	mario->initialvelocity=(int)((game->destinationsize*5+(double)game->acceleration*t*t/2)/t);
	mario->reload=0;
	mario->animate=false;
	return mario;
}

bool MarioAlterSourceRectangle(Object *object)
{
	IDBITMAP id;
	int x=0,count=11;
	Mario *mario=(Mario*)object;
	switch(mario->object.state)
	{
		case stand:
			if(mario->object.facetoleft)
			{
				if(mario->hp==1)
					id=ID_SMALLLEFTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_LEFTFIREMARIO;
					else
						id=ID_LEFTMARIO;
				}
			}
			else
			{
				if(mario->hp==1)
					id=ID_SMALLRIGHTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_RIGHTFIREMARIO;
					else
						id=ID_RIGHTMARIO;
				}
			}
			x=2;
			break;

		case jump:
		case float_:
			if(mario->object.facetoleft)
			{
				if(mario->hp==1)
					id=ID_SMALLLEFTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_LEFTFIREMARIO;
					else
						id=ID_LEFTMARIO;
				}
			}
			else
			{
				if(mario->hp==1)
					id=ID_SMALLRIGHTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_RIGHTFIREMARIO;
					else
						id=ID_RIGHTMARIO;
				}
			}
			x=10;
			break;

		case walk:
			if(mario->object.facetoleft)
			{
				if(mario->hp==1)
					id=ID_SMALLLEFTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_LEFTFIREMARIO;
					else
						id=ID_LEFTMARIO;
				}
			}
			else
			{
				if(mario->hp==1)
					id=ID_SMALLRIGHTMARIO;
				else if(mario->hp==2)
				{
					if(mario->weapon)
						id=ID_RIGHTFIREMARIO;
					else
						id=ID_RIGHTMARIO;
				}
			}
			x=mario->object.duration;
			break;

		case dead:
			id=ID_DEADMARIO;
			x=1;
			count=7;
			break;

		case bigger:
		case smaller:
			if(object->duration)
			{
				int coefficient=0;
				if(object->state==bigger)
					switch(--object->duration/5)
					{
						case 0:
						case 2:
						case 4:
							if(object->facetoleft)
								id=ID_LEFTMARIO;
							else
								id=ID_RIGHTMARIO;
							coefficient=-1;
							break;

						case 1:
						case 3:
							if(object->facetoleft)
								id=ID_SMALLLEFTMARIO;
							else
								id=ID_SMALLRIGHTMARIO;
							coefficient=1;
							break;
						default:
							assert(0);
					}
				else
					switch(--object->duration/5)
					{
						case 0:
						case 2:
						case 4:
							if(object->facetoleft)
								id=ID_SMALLLEFTMARIO;
							else
								id=ID_SMALLRIGHTMARIO;
							coefficient=1;
							break;

						case 1:
						case 3:
							if(object->facetoleft)
								id=ID_LEFTMARIO;
							else
								id=ID_RIGHTMARIO;
							coefficient=-1;
							break;
						default:
							assert(0);
					}

				if(object->id!=id)
					object->destination.top+=object->game->destinationsize*coefficient;
				switch(mario->laststate)
				{
					case stand:
						x=2;
						break;

					case jump:
					case float_:
						x=10;
						break;

					case walk:
						x=mario->lastduration;
						break;
				}
			}
			else
			{
				if(object->state==smaller)
					mario->invincible=80;
				object->state=mario->laststate;
				object->duration=mario->lastduration;
				object->game->pause=false;
				return true;
			}
			break;
		default:
			return true;
	}
	if(object->id!=id)
	{
		object->ReleaseImage(object);
		object->id=id;
		if(!object->GetImage(object))
			return false;
	}
	CommonAlter(object,x,count);
	if(mario->invincible)
	{
		if(mario->invincible/5%2)
		{
			object->source.left=0;
			object->source.top=0;
			object->source.right=0;
			object->source.bottom=0;
		}
	}
	return true;
}

void MarioMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	Mario *mario=(Mario*)object;
	D2D1_RECT_F *destination=&object->destination;
	if(object->state==hide)
		return;
	if(mario->animate)
	{
		if(object->state==jump)
		{
			if(!MoveIfValidate(object,chain,10,1,true))
			{
				while(MoveIfValidate(object,chain,1,1,true));
				object->state=stand;
			}
		}
		else
		{
			for(int i=0;i<256;i++)
				keyboard[i]=0;
			keyboard['D']=0x80;
		}
	}

	if((keyboard['A']&0x80)||(keyboard['D']&0x80))
	{
		int coefficient=0;
		if((keyboard['A']&0x80)&&!(keyboard['D']&0x80))
		{
			object->facetoleft=true;
			coefficient=-1;
		}
		else if(!(keyboard['A']&0x80)&&(keyboard['D']&0x80))
		{
			object->facetoleft=false;
			coefficient=1;
		}
		if(coefficient)
		{
			if(object->state!=jump&&object->state!=float_)
			{
				if(object->state==walk)
				{
					object->duration++;
					object->duration%=10;
				}
				else
				{
					object->state=walk;
					object->duration=0;
				}
			}
			if(MoveIfValidate(object,chain,object->speed,coefficient,false))
			{
				if(object->destination.left<0)
				{
					object->destination.right-=object->destination.left;
					object->destination.left=0;
				}
			}
			else
				while(MoveIfValidate(object,chain,1,coefficient,false));
		}
	}

	/*if(keyboard['S']&0x80)
	{
		bool undo=true;
		destination->top+=object->speed;
		destination->bottom+=object->speed;
		for(Node *node=chain->node;node;node=node->next)
		{
			Object *temp=node->data;
			if(temp->type==type_pipe)
				if(object->Impact(object,temp,true))
				{
					undo=false;
					break;
				}
		}
		if(undo)
		{
			destination->top-=object->speed;
			destination->bottom-=object->speed;
		}
	}*/

	if(object->state==jump)
	{
		int speed;
		speed=mario->initialvelocity-object->game->acceleration*object->falltime;
		if((keyboard['K']&0x80||object->duration>0)&&speed>0)
		{
			if(object->falltime==1)
			{
				PROPVARIANT propvariant={0};
				propvariant.vt=VT_EMPTY;
				Sound *sound=&object->game->sound[mariojump];
				sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
				sound->playing=true;
			}
			object->destination.top-=speed;
			object->destination.bottom-=speed;
			object->falltime++;
			if(object->duration>0)
				object->duration--;
		}
		else
		{
			object->state=float_;
			object->falltime=1;
		}
	}
	else if(object->state!=float_&&keyboard['K']&0x80)
	{
		object->state=jump;
		object->falltime=1;
	}

	if(keyboard['J']&0x80&&mario->weapon&&mario->reload==0)
	{
		mario->reload=30;
		Object *bullet=(Object*)BulletConstructor(object->game,object->facetoleft);
		assert(bullet);
		bullet->AlterSourceRectangle(bullet);
		if(object->facetoleft)
		{
			bullet->destination.right=object->destination.left;
			bullet->destination.left=bullet->destination.right-object->game->destinationsize/2;
		}
		else
		{
			bullet->destination.left=object->destination.right;
			bullet->destination.right=bullet->destination.left+object->game->destinationsize/2;
		}
		bullet->destination.top=object->destination.top;
		bullet->destination.bottom=bullet->destination.top+object->game->destinationsize/2;
		bullet->game->chain->add(bullet->game->chain,bullet);
	}
}

bool MarioImpact(Object *self,Object *others,bool IsValidate)
{
	Mario *mario=(Mario*)self;
	D2D1_RECT_F *rect_first=&self->destination,
		*rect_next=&others->destination;
	if(rect_first->bottom>rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top<rect_next->bottom&&rect_first->left<rect_next->right)
	{
		if(IsValidate)
			return true;
		switch(others->type)
		{
			case type_tortoise:
				if(self->state==float_&&others->state==stand)
				{
					self->state=jump;
					float raisepixel=self->destination.bottom-others->destination.top;
					if(raisepixel<0)
						raisepixel=0;
					self->destination.top-=raisepixel;
					self->destination.bottom-=raisepixel;
					self->falltime=1;
					self->duration=4;
					break;
				}
				else if(others->state==shrink&&!((Tortoise*)others)->moving)
					break;
			case type_mushroommonster:
				if(others->type==type_tortoise||self->state!=float_)
					if(!mario->invincible)
					{
						PROPVARIANT propvariant={0};
						propvariant.vt=VT_EMPTY;
						Sound *sound;
						if(--mario->hp==0)
						{
							self->state=dead;
							self->duration=100;
							sound=&self->game->sound[killedbymonster];
							sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
							sound->playing=true;
						}
						else
						{
							mario->laststate=self->state;
							mario->lastduration=self->duration;
							mario->weapon=false;
							self->state=smaller;
							self->duration=25;
							sound=&self->game->sound[mariosmaller];
							sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
							sound->playing=true;
						}
					}
				break;

			case type_mushroom:
				if(others->state==stand)
				{
					mario->hp++;
					mario->laststate=self->state;
					mario->lastduration=self->duration;
					self->state=bigger;
					self->duration=25;
				}
				break;

			case type_flower:
				if(others->state==stand)
					mario->weapon=true;
				break;

			case type_star:
				if(others->state==stand)
					mario->invincible=800;
				break;

			case type_flag:
				mario->animate=true;
				break;

			case type_castle:
				self->priority=0;
				self->state=hide;
				mario->animate=false;
				break;
		}
	}
	return false;
}

void MarioGravity(Object *object,Chain *chain)
{
	if(object->state!=jump)
		if(MoveIfValidate(object,chain,object->game->acceleration*object->falltime,1,true))
		{
			object->state=float_;
			object->falltime++;
		}
		else
		{
			if(object->state==float_)
			{
				object->state=stand;
				object->duration=0;
				object->falltime=1;
			}
			while(MoveIfValidate(object,chain,1,1,true));
		}
}

void MarioReaction(Object *object)
{
	Mario *mario=(Mario*)object;
	if(mario->invincible)
		mario->invincible--;
	if(mario->reload)
		mario->reload--;
	switch(object->state)
	{
		case jump:
		{
			bool impact;
			do
			{
				impact=false;
				for(Node *node=object->game->chain->node;node;node=node->next)
				{
					Object *temp=node->data;
					if(temp->type==type_box||
						temp->type==type_brick)
						if(object->Impact(object,temp,true))
						{
							object->destination.top+=1;
							object->destination.bottom+=1;
							impact=true;
							object->state=float_;
							object->falltime=1;
							break;
						}
				}
			} while(impact);
		}
		break;

		case bigger:
		case smaller:
			object->game->pause=true;
			break;

		case dead:
			if(--object->duration>75)
			{
				object->destination.top-=object->speed;
				object->destination.bottom-=object->speed;
			}
			else
			{
				object->destination.top+=object->speed;
				object->destination.bottom+=object->speed;
			}
			object->game->pause=true;
			break;
	}
}

void * MarioUpdateData(Object *object,bool update)
{
	static Mario temp;
	if(update)
	{
		*((Mario*)object)=temp;
		return NULL;
	}
	else
	{
		temp=*((Mario*)object);
		return &temp;
	}
}