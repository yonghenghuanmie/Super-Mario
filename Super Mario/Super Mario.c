#include <stdbool.h>
#include <assert.h>
#include <tchar.h>
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <mfapi.h>
#include <mfidl.h>
#include "Chain.h"
#include "Draw.h"
#include "Sound.h"
#include "Map.h"
#include "Object.h"
#include "Mario.h"
#include "MushroomMonster.h"
#include "Tortoise.h"
#include "Coin.h"
#include "Box.h"
#include "Brick.h"
#include "Pipe.h"
#include "Flag.h"
#include "Castle.h"
#include "Ornament.h"
#include "Background.h"
#include "../WallPaper Window/WallPaper Window.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"..\\x64\\DEBUG\\WallPaper Window.lib")
#else
#pragma comment(lib,"..\\x64\\RELEASE\\WallPaper Window.lib")
#endif // _DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"..\\DEBUG\\WallPaper Window.lib")
#else
#pragma comment(lib,"..\\RELEASE\\WallPaper Window.lib")
#endif // _DEBUG
#endif // _WIN64

void LoadLine(Game *game,WORD map[][CountY])
{
	Object *object=NULL;
	for(int y=0;y<CountY;y++,object=NULL)
	{
		switch(LOBYTE(map[game->current][y]))
		{
			case type_mushroommonster:
				object=(Object*)MushroomMonsterConstructor(game);
				assert(object);
				break;

			case type_tortoise:
				object=(Object*)TortoiseConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_coin:
				object=(Object*)CoinConstructor(game);
				assert(object);
				break;

			case type_box:
				object=(Object*)BoxConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_brick:
				object=(Object*)BrickConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_pipe:
				object=(Object*)PipeConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_flag:
				object=(Object*)FlagConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_castle:
				object=(Object*)CastleConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;

			case type_ornament:
				object=(Object*)OrnamentConstructor(game,HIBYTE(map[game->current][y]));
				assert(object);
				break;
		}
		if(object)
		{
			object->AlterSourceRectangle(object);
			object->SetDestination(object,game->current,y,game->destinationsize);
			object->destination.left-=game->offset;
			object->destination.right-=game->offset;
			game->chain->add(game->chain,object);
		}
	}
}

void LoadMap(Game *game)
{
	bool exist=false;
	for(Node *node=game->chain->node;node;)
	{
		Object *object=(Object*)node->data;
		if(object->state!=hide)
		{
			node=game->chain->remove(game->chain,node);
			object->Destructor(object);
			continue;
		}
		else
		{
			object->priority=2;
			object->state=stand;
			object->destination=(D2D1_RECT_F){ 0,0,(float)game->destinationsize,(float)game->destinationsize*(((Mario*)object)->hp==1?1:2) };
			exist=true;
		}
		node=node->next;
	}
	game->newstage=false;
	game->current=0;
	game->offset=0;

	if(!exist)
	{
		Mario *mario=MarioConstructor(game);
		assert(mario);
		assert(mario->object.AlterSourceRectangle(&mario->object));
		mario->object.SetDestination(&mario->object,3,12,game->destinationsize);
		mario->object.destination.right=0.9*game->destinationsize+mario->object.destination.left;
		game->chain->add(game->chain,mario);
	}
	Background *background=BackgroundConstructor(game);
	assert(background);
	assert(background->object.AlterSourceRectangle(&background->object));
	game->chain->add(game->chain,background);
	for(int x=0;x<game->cx/game->destinationsize+1;x++)
	{
		LoadLine(game,(WORD(*)[15])game->map[game->point-1]);
		game->current++;
	}
}

bool Initialize(HWND hwnd,void *Parameter)
{
	Game *game=(Game*)Parameter;
	MFStartup(MF_VERSION,MFSTARTUP_FULL);
	*game=(Game){
		1,false,{(WORD*)map1},3,0,false,hwnd,DrawConstructor(hwnd),GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
			0,0,GetSystemMetrics(SM_CYSCREEN)/CountY,{0,0,357,0,0,340,323,238},17,2,ChainConstructor(),SoundConstructor()
	};
	assert(game->sound);
	LoadMap(game);
	return true;
}

bool Close(void *Parameter)
{
	Game *game=(Game*)Parameter;
	for(Node *node=game->chain->node;node;node=node->next)
		((Object*)node->data)->Destructor((Object*)node->data);
	SoundDestrutor(game->sound);
	game->chain->Destructor(game->chain);
	game->draw->Destructor(game->draw);
	MFShutdown();
	return true;
}

bool Paint(void *Parameter)
{
	Game *game=(Game*)Parameter;
	PAINTSTRUCT PaintStruct;
	HDC hdc=BeginPaint(game->hwnd,&PaintStruct);
	game->draw->RenderTarget->lpVtbl->Base.BeginDraw((ID2D1RenderTarget*)game->draw->RenderTarget);
	for(Node *node=game->chain->node;node;node=node->next)
	{
		Object *object=(Object*)node->data;
		if(object->type==type_background)
		{
			for(int i=0;i<game->cx/game->destinationsize+1;i++)
				for(int j=0;j<CountY;j++)
				{
					object->SetDestination(object,i,j,game->destinationsize);
					game->draw->RenderTarget->lpVtbl->Base.DrawBitmap((ID2D1RenderTarget*)game->draw->RenderTarget,object->D2DBitmap,
						&object->destination,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,&object->source);
				}
			break;
		}
	}

	for(int i=0;i<MAXPRIORITY;i++)
		for(Node *node=game->chain->node;node;node=node->next)
		{
			Object *object=(Object*)node->data;
			if(object->type==type_background)
				continue;
			if(object->priority==i&&(object->state!=dead||object->duration!=0))
				game->draw->RenderTarget->lpVtbl->Base.DrawBitmap((ID2D1RenderTarget*)game->draw->RenderTarget,object->D2DBitmap,
					&object->destination,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,&object->source);
		}

#ifdef UNICODE
	ID2D1SolidColorBrush *SolidBrush;
	game->draw->RenderTarget->lpVtbl->Base.CreateSolidColorBrush((ID2D1RenderTarget*)game->draw->RenderTarget,
		&(D2D1_COLOR_F){ 255,255,255,1 },NULL,&SolidBrush);
	TCHAR string[16];
	_stprintf(string,_T("关卡:%d"),game->point);
	float x=(float)(0.25*game->cx),y=(float)(0.05*game->cy);
	D2D1_RECT_F rect={x,y,x+200,y+100};
	game->draw->RenderTarget->lpVtbl->Base.DrawText((ID2D1RenderTarget*)game->draw->RenderTarget,string,(int)_tcslen(string),game->draw->DWriteTextFormat,
		&rect,(ID2D1Brush*)SolidBrush,D2D1_DRAW_TEXT_OPTIONS_NONE,DWRITE_MEASURING_MODE_NATURAL);
	_stprintf(string,_T("生命:%d"),game->life);
	x=(float)(0.25*game->cx)*2;
	rect=(D2D1_RECT_F){ x,y,x+200,y+100 };
	game->draw->RenderTarget->lpVtbl->Base.DrawText((ID2D1RenderTarget*)game->draw->RenderTarget,string,(int)_tcslen(string),game->draw->DWriteTextFormat,
		&rect,(ID2D1Brush*)SolidBrush,D2D1_DRAW_TEXT_OPTIONS_NONE,DWRITE_MEASURING_MODE_NATURAL);
	_stprintf(string,_T("分数:%d"),game->totalgrade);
	x=(float)(0.25*game->cx)*3;
	rect=(D2D1_RECT_F){ x,y,x+200,y+100 };
	game->draw->RenderTarget->lpVtbl->Base.DrawText((ID2D1RenderTarget*)game->draw->RenderTarget,string,(int)_tcslen(string),game->draw->DWriteTextFormat,
		&rect,(ID2D1Brush*)SolidBrush,D2D1_DRAW_TEXT_OPTIONS_NONE,DWRITE_MEASURING_MODE_NATURAL);
#endif // UNICODE

	game->draw->RenderTarget->lpVtbl->Base.EndDraw((ID2D1RenderTarget*)game->draw->RenderTarget,NULL,NULL);
	EndPaint(game->hwnd,&PaintStruct);
	return true;
}

bool Timer(POINT *mouse,unsigned char keyboard[256],void *Parameter)
{
	Game *game=(Game*)Parameter;
	if(!game->pause)
	{
		int offset=0;
		for(Node *node=game->chain->node;node;node=node->next)
		{
			Object *object=node->data;
			if(object->state!=dead)
			{
				if(object->Move)
					object->Move(object,game->chain,keyboard);
				if(object->Gravity)
					object->Gravity(object,game->chain);
				if(object->type==type_mario&&object->destination.left>game->cx*0.4)
					offset=object->speed;
			}
		}

		if(offset)
		{
			for(Node *node=game->chain->node;node;node=node->next)
			{
				Object *object=node->data;
				object->destination.left-=offset;
				object->destination.right-=offset;
			}
			game->offset+=offset;

			if(game->current*game->destinationsize-game->offset<game->cx-1)
			{
				LoadLine(game,(WORD(*)[15])game->map[game->point-1]);
				game->current++;
			}
		}
	}

	for(Node *node=game->chain->node;node;)
	{
		Object *object=node->data;
		if(object->OutOfRange(object,game->cx,game->cy)||(object->state==dead&&object->duration==0))
		{
			if(object->type==type_mario)
			{
				game->pause=false;
				if(--game->life!=0)
				{
					object->state=float_;
					object->destination=(D2D1_RECT_F){ 0,0,(float)game->destinationsize*0.9,(float)game->destinationsize };
					((Mario*)object)->hp=1;
					((Mario*)object)->invincible=100;
					continue;
				}
				else
				{
					game->point=1;
					game->newstage=true;
					game->life=3;
					game->totalgrade=0;
					PROPVARIANT propvariant={0};
					propvariant.vt=VT_EMPTY;
					Sound *sound=&object->game->sound[backgroundmusic];
					sound->session->lpVtbl->Stop(sound->session);
					sound->playing=false;
					sound=&object->game->sound[gameover];
					sound->session->lpVtbl->Start(sound->session,&GUID_NULL,&propvariant);
					sound->playing=true;
					Sleep(3000);
				}
			}
			node=game->chain->remove(game->chain,node);
			object->Destructor(object);
			continue;
		}
		node=node->next;
	}

	if(!game->pause)
		for(Node *node=game->chain->node;node;node=node->next)
		{
			Object *object=node->data;
			if(object->Impact&&object->state!=dead)
				for(Node *temp=node->next;temp;temp=temp->next)
				{
					Object *others=temp->data;
					if(others->state!=dead)
					{
						object->Impact(object->UpdateData(object,false),others,false);
						if(others->Impact)
						{
							others->Impact(others->UpdateData(others,false),object,false);
							others->UpdateData(others,true);
						}
						object->UpdateData(object,true);
					}
				}
		}

	for(Node *node=game->chain->node;node;node=node->next)
	{
		Object *object=node->data;
		if(object->Reaction)
			object->Reaction(object);
		if(object->AlterSourceRectangle)
			object->AlterSourceRectangle(object);
	}

	//???
	if(game->newstage)
		LoadMap(game);
	InvalidateRect(game->hwnd,NULL,false);
	UpdateWindow(game->hwnd);
	return true;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	Game game;
	WALLPAPERWINDOW WallPaperWindow={0};
	WallPaperWindow.Initialize			=Initialize;
	WallPaperWindow.InitializeParameter	=&game;
	WallPaperWindow.Close				=Close;
	WallPaperWindow.CloseParameter		=&game;
	WallPaperWindow.Paint				=Paint;
	WallPaperWindow.PaintParameter		=&game;
	WallPaperWindow.TimeInterval		=10;
	WallPaperWindow.Timer				=Timer;
	WallPaperWindow.TimerParameter		=&game;
	CreateWallpaperWindow(&WallPaperWindow);
	return 0;
}