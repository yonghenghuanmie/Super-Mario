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
#include "Pipe.h"

bool PipeAlterSourceRectangle(Object*);

Pipe * PipeConstructor(Game * game,ChildImage id)
{
	Pipe *pipe=calloc(1,sizeof(Pipe));
	if(!ObjectConstructor((Object*)pipe,game,ID_BACKGROUND))
	{
		free(pipe);
		return NULL;
	}
	pipe->object.type=type_pipe;
	pipe->object.priority=2;
	pipe->object.AlterSourceRectangle=PipeAlterSourceRectangle;
	pipe->object.Gravity=NULL;
	pipe->location=RT;
	pipe->id=id;
	return pipe;
}

bool PipeAlterSourceRectangle(Object *object)
{
	int x,y;
	Pipe *pipe=(Pipe*)object;
	ChildImage id;
	switch(object->game->point)
	{
		case 1:
			id=greenpipe1;
			x=5;
			y=0;
			break;
	}
	switch(pipe->id-id)
	{
		case 0:
			x+=0;
			y+=2;
			break;
		case 1:
			x+=0;
			y+=3;
			break;
		case 2:
			x+=1;
			y+=2;
			break;
		case 3:
			x+=1;
			y+=3;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			x+=2;
			y+=pipe->id-id-4;
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			x+=3;
			y+=pipe->id-id-9;
			break;
	}
	BackgroundCommonAlter(object,pipe->location,x,y);
	return true;
}
