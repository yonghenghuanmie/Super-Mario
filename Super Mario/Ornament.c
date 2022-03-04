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
#include "Ornament.h"

bool OrnamentAlterSourceRectangle(Object*);

Ornament * OrnamentConstructor(Game * game,ChildImage id)
{
	Ornament *ornament=calloc(1,sizeof(Ornament));
	if(!ObjectConstructor((Object*)ornament,game,ID_BACKGROUND))
	{
		free(ornament);
		return NULL;
	}
	ornament->object.type=type_ornament;
	ornament->object.priority=0;
	ornament->object.AlterSourceRectangle=OrnamentAlterSourceRectangle;
	ornament->object.Gravity=NULL;
	ornament->location=LT;
	ornament->id=id;
	return ornament;
}

bool OrnamentAlterSourceRectangle(Object *object)
{
	int x,y;
	Ornament *ornament=(Ornament*)object;
	if(ornament->id>=mountain1_1&&ornament->id<=mountain2_9)
		switch(ornament->id)
		{
			case mountain1_1:
				x=3,y=2;
				break;
			case mountain1_2:
				x=4,y=1;
				break;
			case mountain1_3:
				x=4,y=2;
				break;
			case mountain1_4:
				x=5,y=2;
				break;
			case mountain2_1:
				x=6,y=2;
				break;
			case mountain2_2:
				x=7,y=1;
				break;
			case mountain2_3:
				x=7,y=2;
				break;
			case mountain2_4:
				x=8,y=0;
				break;
			case mountain2_5:
				x=8,y=1;
				break;
			case mountain2_6:
				x=8,y=2;
				break;
			case mountain2_7:
				x=9,y=1;
				break;
			case mountain2_8:
				x=9,y=2;
				break;
			case mountain2_9:
				x=10,y=2;
				break;
		}
	else if(ornament->id<=redcloud3_4)
	{
		if(ornament->id<=bluecloud3_4)
			x=ornament->id-bluecloud1_1;
		else
			x=ornament->id-redcloud1_1;
		y=x%2+3;
		x=x/2+3;
		if(ornament->id>bluecloud3_4)
			y+=2;
	}
	else if(ornament->id<=tree3_3)
	{
		x=ornament->id-tree1_1+3;
		y=7;
	}
	else assert(0);
	BackgroundCommonAlter(object,ornament->location,x,y);
	return true;
}