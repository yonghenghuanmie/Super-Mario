#pragma once
#include "Macro.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

typedef struct WALLPAPERWINDOW
{
	bool (*Initialize)(IN HWND,IN OPTIONAL void*);
	void *InitializeParameter;
	bool (*Close)(IN OPTIONAL void*);
	void *CloseParameter;
	bool (*Paint)(IN OPTIONAL void*);
	void *PaintParameter;
	unsigned long TimeInterval;//ms
	bool (*Timer)(IN POINT*,IN unsigned char[256],IN OPTIONAL void*);
	void *TimerParameter;
	void (*ErrorProcess)(IN void*,IN OPTIONAL void*);
	void *ErrorProcessParameter;
}WALLPAPERWINDOW;

declaration bool CreateWallpaperWindow(WALLPAPERWINDOW*);