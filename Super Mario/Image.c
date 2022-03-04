#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <tchar.h>
#include <d2d1.h>
#include <wincodec.h>
#include "Draw.h"
#include "Chain.h"
#include "Map.h"
#include "Image.h"

void ImageDestructor(Image*);
bool GetImage(Image*);
void ReleaseImage(Image*);
bool ImageRemoveRectangle(Image*,RECT*);
void ImageSetDestination(Image*,int,int,int);
bool OutOfRange(Image*,int,int);

bool ImageConstructor(Image *image,Draw *draw,IDBITMAP id,TYPE type)
{
	if(image==NULL)
		return false;

	image->id=id;
	image->type=type;
	image->draw=draw;
	image->Destructor=ImageDestructor;
	image->GetImage=GetImage;
	image->ReleaseImage=ReleaseImage;
	//image->AddRectangle=NULL;
	//image->RemoveRectangle=ImageRemoveRectangle;
	image->SetDestination=ImageSetDestination;
	image->AlterSourceRectangle=NULL;
	image->OutOfRange=OutOfRange;
	if(!image->GetImage(image))
		return false;
	return true;
}

void ImageDestructor(Image *object)
{
	object->ReleaseImage(object);
	free(object);
}

int counter[ID_BACKGROUND+1];
bool GetImage(Image *image)
{
	static ID2D1Bitmap *D2DBitmap[ID_BACKGROUND+1];
	if(counter[image->id]==0)
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
			_T("Resources\\fireLeft.png"),
			_T("Resources\\fireRight.png"),
			_T("Resources\\coinani.png"),
			_T("Resources\\superMarioMap.png")
		};
		image->D2DBitmap=image->draw->GetD2DBitmap(image->draw,path[image->id]);
		if(image->D2DBitmap==NULL)
			return false;
		D2DBitmap[image->id]=image->D2DBitmap;
	}
	else
		image->D2DBitmap=D2DBitmap[image->id];
	counter[image->id]++;
	return true;
}

void ReleaseImage(Image *image)
{
	if(--counter[image->id]==0)
		image->D2DBitmap->lpVtbl->Base.Base.Base.Release((IUnknown*)image->D2DBitmap);
}

//bool ImageRemoveRectangle(Image *image,RECT *compared)
//{
//	bool success=false;
//	for(Node *source=image->source->node,
//		*destination=image->destination->node;
//		source!=NULL;
//		source=source->next,
//		destination=destination->next)
//	{
//		RECT *temp=(RECT*)destination->data;
//		if(compared==temp||
//			(temp->left==compared->left&&
//				temp->top==compared->top&&
//				temp->right==compared->right&&
//				temp->bottom==compared->bottom))
//		{
//			assert(image->source->remove(image->source,source));
//			assert(image->destination->remove(image->destination,destination));
//			success=true;
//			break;
//		}
//	}
//	return success;
//}

void ImageSetDestination(Image *image,int x,int y,int size)
{
	image->destination.left=(float)x*size;
	image->destination.top=(float)y*size;
	//It should be left/top +size-1,but in fact right/bottom will not be draw
	image->destination.right=(float)(x+1)*size;
	image->destination.bottom=(float)(y+1)*size;
}

bool OutOfRange(Image *image,int cx,int cy)
{
	bool out=true;
	D2D1_RECT_F *rect_first=&image->destination,
		*rect_next=&(D2D1_RECT_F){ 0,0,(float)cx-1,(float)cy-1 };
	if(rect_first->bottom>rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top<rect_next->bottom&&rect_first->left<rect_next->right)
		out=false;
	return out;
}