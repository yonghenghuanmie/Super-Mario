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

void ObjectDestructor(Object*);
bool GetImage(Object*);
void ReleaseImage(Object*);
void SetDestination(Object*,int,int,int);
bool OutOfRange(Object*,int,int);
void Gravity(Object*,Chain*);

bool ObjectConstructor(Object *object,Game *game,IDBITMAP id)
{
	assert(object);
	object->game=game;
	object->id=id;
	object->state=stand;
	object->falltime=1;
	object->Destructor=ObjectDestructor;
	object->GetImage=GetImage;
	object->ReleaseImage=ReleaseImage;
	object->SetDestination=SetDestination;
	object->AlterSourceRectangle=NULL;
	object->OutOfRange=OutOfRange;
	object->Move=NULL;
	object->Impact=NULL;
	object->Gravity=Gravity;
	object->Reaction=NULL;
	object->UpdateData=NULL;
	if(GetImage(object))
		return true;
	return false;
}

void ObjectDestructor(Object *object)
{
	object->ReleaseImage(object);
	free(object);
}

int counter[ID_BACKGROUND+1];
bool GetImage(Object *object)
{
	static ID2D1Bitmap *D2DBitmap[ID_BACKGROUND+1];
	if(counter[object->id]==0)
	{
		static TCHAR *path[]=
		{
			_T("Resources\\smallWalkLeft.png"),
			_T("Resources\\smallWalkRight.png"),
			_T("Resources\\small_die.png"),
			_T("Resources\\walkLeft.png"),
			_T("Resources\\walkRight.png"),
			_T("Resources\\WalkLeft_fire.png"),
			_T("Resources\\WalkRight_fire.png"),
			_T("Resources\\Mushroom0.png"),
			_T("Resources\\tortoise0.png"),
			_T("Resources\\flower0.png"),
			_T("Resources\\rewardMushroomSet.png"),
			_T("Resources\\Tools.png"),
			_T("Resources\\fireRight.png"),
			_T("Resources\\coinani.png"),
			_T("Resources\\superMarioMap.png")
		};
		object->D2DBitmap=object->game->draw->GetD2DBitmap(object->game->draw,path[object->id]);
		if(object->D2DBitmap==NULL)
			return false;
		D2DBitmap[object->id]=object->D2DBitmap;
	}
	else
		object->D2DBitmap=D2DBitmap[object->id];
	counter[object->id]++;
	return true;
}

void ReleaseImage(Object *object)
{
	if(--counter[object->id]==0)
		object->D2DBitmap->lpVtbl->Base.Base.Base.Release((IUnknown*)object->D2DBitmap);
}

void SetDestination(Object *object,int x,int y,int size)
{
	object->destination.left=(float)x*size;
	object->destination.top=(float)y*size;
	//It should be left/top +size-1,but in fact right/bottom will not be draw
	object->destination.right=(float)(x+1)*size;
	object->destination.bottom=(float)(y+1)*size;
}

bool OutOfRange(Object *object,int cx,int cy)
{
	bool out=true;
	D2D1_RECT_F *rect_first=&object->destination,
		*rect_next=&(D2D1_RECT_F){ 0,0,(float)cx-1,(float)cy-1 };
	if(rect_first->bottom>rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top<rect_next->bottom&&rect_first->left<rect_next->right)
		out=false;
	return out;
}

D2D1_SIZE_U GetPixelSize(ID2D1Bitmap*,D2D1_SIZE_U*);
void CommonAlter(Object *object,int x,int count)
{
	//D2D1_SIZE_U size=object->D2DBitmap->lpVtbl->GetPixelSize(object->D2DBitmap);
	D2D1_SIZE_U size;
	GetPixelSize(object->D2DBitmap,&size);
	object->source.left=(float)x*size.width/count;
	object->source.top=(float)0;
	object->source.right=(float)(x+1)*size.width/count;
	object->source.bottom=(float)size.height;
}

void BackgroundCommonAlter(Object *object,Location location,int x,int y)
{
	object->source.left=(float)object->game->start[location].x+x*object->game->sourcesize+1;
	object->source.top=(float)object->game->start[location].y+y*object->game->sourcesize+1;
	object->source.right=(float)object->game->start[location].x+(x+1)*object->game->sourcesize;
	object->source.bottom=(float)object->game->start[location].y+(y+1)*object->game->sourcesize;
}

bool MoveIfValidate(Object *object,Chain *chain,int speed,int coefficient,bool updown)
{
	bool success=true;
	float *param1,*param2;
	if(updown)
	{
		param1=&object->destination.top;
		param2=&object->destination.bottom;
	}
	else
	{
		param1=&object->destination.left;
		param2=&object->destination.right;
	}
	*param1+=speed*coefficient;
	*param2+=speed*coefficient;
	for(Node *node=chain->node;node;node=node->next)
	{
		Object *temp=node->data;
		if(temp->type==type_box||
			temp->type==type_brick||
			temp->type==type_pipe)
			if(object->Impact(object,temp,true))
			{
				*param1-=speed*coefficient;
				*param2-=speed*coefficient;
				success=false;
				break;
			}
	}
	return success;
}

void Gravity(Object *object,Chain *chain)
{
	if(MoveIfValidate(object,chain,object->game->acceleration*object->falltime,1,true))
		object->falltime++;
	else
	{
		object->falltime=1;
		while(MoveIfValidate(object,chain,1,1,true));
	}
}