#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <d2d1.h>
#include <wincodec.h>
#include "Chain.h"
#include "Draw.h"
#include "Image.h"
#include "Object.h"
#include "Monster.h"

bool MushroomAlterSourceRectangle(Image*);
void MushroomMonsterMove(MushroomMonster*,Chain*,unsigned char[256]);
bool MushroomMonsterImpact(MushroomMonster*,Image*);

MushroomMonster* MushroomMonsterConstructor(Draw *draw)
{
	MushroomMonster *mushroommonster=DefaultConstructor(draw,ID_MUSHROOMMONSTER,type_mushroommonster,sizeof(MushroomMonster));
	if(mushroommonster==NULL)
		return NULL;

	mushroommonster->movable.state=walk;
	mushroommonster->movable.speed=8;
	mushroommonster->movable.move=MushroomMonsterMove;
	mushroommonster->movable.impact=MushroomMonsterImpact;
	mushroommonster->AlterSourceRectangle=MushroomAlterSourceRectangle;
	return mushroommonster;
}

bool MushroomAlterSourceRectangle(Image *image)
{
	int x=0,count=4;
	MushroomMonster *mushroommonster=(MushroomMonster*)image;
	switch(mushroommonster->movable.state)
	{
		case walk:
			if(mushroommonster->movable.FaceToLeft)
				x=1;
			else
				x=0;
			break;

		case dead:
			x=2;
			break;
	}
	CommonAlter(image,x,count);
	return true;
}

void MushroomMonsterMove(MushroomMonster *mushroommonster,Chain *chain,unsigned char keyboard[256])
{
	D2D1_RECT_F *destination=&mushroommonster->movable.image.destination;
	int direction=1;
	if(mushroommonster->movable.FaceToLeft)
		direction=-1;
		destination->left+=mushroommonster->movable.speed*direction;
		destination->right+=mushroommonster->movable.speed*direction;
		for(Node *node=chain->node;node;node=node->next)
		{
			Image *image=node->data;
			if(image->type==type_mushroommonster||
				image->type==type_tortoise||
				image->type==type_box||
				image->type==type_brick||
				image->type==type_pipe)
				if(mushroommonster->movable.impact(mushroommonster,image))
				{
					destination-=2*mushroommonster->movable.speed*direction;
					destination-=2*mushroommonster->movable.speed*direction;
					break;
				}
		}
}

bool MushroomMonsterImpact(MushroomMonster *mushroommonster,Image *image)
{
	bool impact=false;
	D2D1_RECT_F *rect_first=&mushroommonster->movable.image.destination,
		*rect_next=&image->destination;
	if(rect_first->bottom<rect_next->top&&rect_first->right>rect_next->left&&
		rect_first->top>rect_next->bottom&&rect_first->left<rect_next->right)
			{
				switch(image->type)
				{
					case type_mario:
						if(((Movable*)image)->state==float_)
							mushroommonster->movable.state=dead;
						break;

					case type_mushroommonster:
						mushroommonster->movable.FaceToLeft=!mushroommonster->movable.FaceToLeft;
						impact=true;
						break;

					case type_tortoise:
						if(((Movable*)image)->state==shrink)
							mushroommonster->movable.state=dead;
						else
						{
							mushroommonster->movable.FaceToLeft=!mushroommonster->movable.FaceToLeft;
							impact=true;
						}
						break;

					case type_bullet:
						mushroommonster->movable.state=dead;
						break;

					case type_box:
					case type_brick:
					case type_pipe:
						mushroommonster->movable.FaceToLeft=!mushroommonster->movable.FaceToLeft;
						impact=true;
						break;
				}
			}
	return impact;
}