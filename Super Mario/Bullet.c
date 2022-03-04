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
#include "Bullet.h"

bool BulletAlterSourceRectangle(Object*);
void BulletMove(Object*,Chain*,unsigned char[256]);
void BulletGravity(Object*,Chain*);
bool BulletImpact(Object*,Object*,bool);
void BulletReaction(Object*);
void* BulletUpdateData(Object*,bool);

Bullet * BulletConstructor(Game * game,bool facetoleft)
{
	Bullet *bullet=calloc(1,sizeof(Bullet));
	if(!ObjectConstructor((Object*)bullet,game,ID_BULLET))
	{
		free(bullet);
		return NULL;
	}
	bullet->object.type=type_bullet;
	bullet->object.priority=2;
	bullet->object.facetoleft=facetoleft;
	bullet->object.speed=15;
	bullet->object.duration=0;
	bullet->object.AlterSourceRectangle=BulletAlterSourceRectangle;
	bullet->object.Move=BulletMove;
	bullet->object.Gravity=BulletGravity;
	bullet->object.Impact=BulletImpact;
	bullet->object.Reaction=BulletReaction;
	bullet->object.UpdateData=BulletUpdateData;
	bullet->initialvelocity=0;
	PROPVARIANT propvariant={0};
	propvariant.vt=VT_EMPTY;
	Sound *sound=&game->sound[shot];
	sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
	sound->playing=true;
	return bullet;
}

bool BulletAlterSourceRectangle(Object *object)
{
	int x,count=11;
	x=object->duration/5;
	if(object->state==dead)
		x+=8;
	if(x<9)
	{
		object->source.left=(float)x*10;
		object->source.top=3;
		object->source.right=(float)(x+1)*10;
		object->source.bottom=12;
	}
	else
	{
		CommonAlter(object,10,count);
		switch(x)
		{
			case 9:
				object->source.left=90;
				object->source.right=103;
				break;
			case 10:
				object->source.left=103;
				break;
		}
	}
	return true;
}

void BulletMove(Object *object,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&object->destination;
	int coefficient;
	if(object->facetoleft)
		coefficient=-1;
	else
		coefficient=1;
	if(!MoveIfValidate(object,chain,object->speed,coefficient,false))
	{
		object->state=dead;
		object->duration=15;
	}
}

void BulletGravity(Object *object,Chain *chain)
{
	Bullet* bullet=(Bullet*)object;
	if(MoveIfValidate(object,chain,object->game->acceleration*object->falltime-bullet->initialvelocity,1,true))
		object->falltime++;
	else
	{
		object->falltime=1;
		if(object->game->acceleration*object->falltime-bullet->initialvelocity>0)
			bullet->initialvelocity=20;
		else
			bullet->initialvelocity=0;
	}
}

bool BulletImpact(Object *self,Object *others,bool IsValidate)
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
			case type_mushroommonster:
			case type_tortoise:
				self->state=dead;
				self->duration=15;
				break;
		}
	}
	return false;
}

void BulletReaction(Object *object)
{
	switch(object->state)
	{
		case stand:
			object->duration++;
			object->duration%=40;
			break;

		case dead:
			if(object->duration>0)
				object->duration--;
			break;
	}
}

void * BulletUpdateData(Object *object,bool update)
{
	static Bullet temp[2],*pointer;
	if(update)
	{
		*((Bullet*)object)=*pointer;
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
		*pointer=*((Bullet*)object);
		return pointer;
	}
}
