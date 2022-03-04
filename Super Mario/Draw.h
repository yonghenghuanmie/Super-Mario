#pragma once

typedef struct Draw
{
	IWICImagingFactory *ImagingFactory;
	ID2D1HwndRenderTarget *RenderTarget;
	IDWriteTextFormat *DWriteTextFormat;

	void (*Destructor)(struct Draw*);
	ID2D1Bitmap* (*GetD2DBitmap)(struct Draw*,TCHAR*);
}
Draw;

#ifdef __cplusplus
extern "C" Draw* DrawConstructor(HWND);
#else
Draw* DrawConstructor(HWND);
#endif // __cplusplus