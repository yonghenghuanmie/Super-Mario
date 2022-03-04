#include <stdbool.h>
#include <stdlib.h>
#include <tchar.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include "Draw.h"

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Dwrite.lib")
#pragma comment(lib,"Windowscodecs.lib")

void DrawDestructor(Draw*);
ID2D1Bitmap* GetD2DBitmap(Draw*,TCHAR*);

Draw* DrawConstructor(HWND hwnd)
{
	Draw *draw=(Draw*)calloc(1,sizeof(Draw));
	if(draw==NULL)
		return NULL;

	bool success=false;
	CoInitialize(NULL);
	if(SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory1,NULL,CLSCTX_INPROC_SERVER,IID_IWICImagingFactory,(void**)&draw->ImagingFactory)))
	{
		ID2D1Factory *D2DFactory;
		if(SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,IID_ID2D1Factory,(void**)&D2DFactory)))
		{
			D2D1_RENDER_TARGET_PROPERTIES Properties;
			Properties.type					= D2D1_RENDER_TARGET_TYPE_DEFAULT;
			Properties.pixelFormat.format	= DXGI_FORMAT_UNKNOWN;
			Properties.pixelFormat.alphaMode= D2D1_ALPHA_MODE_UNKNOWN;
			Properties.dpiX					= 96.0;
			Properties.dpiY					= 96.0;
			Properties.usage				= D2D1_RENDER_TARGET_USAGE_NONE;
			Properties.minLevel				= D2D1_FEATURE_LEVEL_DEFAULT;

			RECT rect;
			GetClientRect(hwnd,&rect);
			D2D1_HWND_RENDER_TARGET_PROPERTIES HWNDProperties;
			HWNDProperties.hwnd				= hwnd;
			HWNDProperties.pixelSize.width	= rect.right-rect.left;
			HWNDProperties.pixelSize.height	= rect.bottom-rect.top;
			HWNDProperties.presentOptions	= D2D1_PRESENT_OPTIONS_NONE;
			if(SUCCEEDED(D2DFactory->CreateHwndRenderTarget(&Properties,&HWNDProperties,&draw->RenderTarget)))
			{
				GUID IID_IDWriteFactory={0xb859ee5a,0xd838,0x4b5b,0xa2,0xe8,0x1a,0xdc,0x7d,0x93,0xdb,0x48};
				//用下面这种方法的人实在是蛋疼
				/*sscanf("b859ee5a-d838-4b5b-a2e8-1adc7d93db48","%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
				&IID_IDWriteFactory.Data1,&IID_IDWriteFactory.Data2,&IID_IDWriteFactory.Data3,
				&IID_IDWriteFactory.Data4[0],&IID_IDWriteFactory.Data4[1],&IID_IDWriteFactory.Data4[2],&IID_IDWriteFactory.Data4[3],
				&IID_IDWriteFactory.Data4[4],&IID_IDWriteFactory.Data4[5],&IID_IDWriteFactory.Data4[6],&IID_IDWriteFactory.Data4[7]);*/
				IDWriteFactory *DWriteFactory;
				if(SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,IID_IDWriteFactory,(IUnknown**)&DWriteFactory)))
				{
					if(SUCCEEDED(DWriteFactory->CreateTextFormat(L"楷体",NULL,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,
						DWRITE_FONT_STRETCH_NORMAL,28,L"",&draw->DWriteTextFormat)))
						success=true;
					DWriteFactory->Release();
				}
			}
			D2DFactory->Release();
		}
	}

	draw->Destructor=DrawDestructor;
	draw->GetD2DBitmap=GetD2DBitmap;

	if(!success)
	{
		draw->Destructor(draw);
		return NULL;
	}
	return draw;
}

void DrawDestructor(Draw *draw)
{
	if(draw->RenderTarget!=NULL)
		draw->RenderTarget->Release();
	if(draw->ImagingFactory!=NULL)
		draw->ImagingFactory->Release();
	if(draw->DWriteTextFormat!=NULL)
		draw->DWriteTextFormat->Release();
	free(draw);
}

ID2D1Bitmap* GetD2DBitmap(Draw *draw,TCHAR *path)
{
	ID2D1Bitmap *D2DBitmap=NULL;
	HRESULT result;
	IWICBitmapDecoder *BitmapDecoder;
	result=draw->ImagingFactory->CreateDecoderFromFilename(path,NULL,GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,&BitmapDecoder);
	if(SUCCEEDED(result))
	{
		IWICBitmapFrameDecode* FrameDecode;
		result=BitmapDecoder->GetFrame(0,&FrameDecode);
		if(SUCCEEDED(result))
		{
			IWICFormatConverter *FormatConverter;
			result=draw->ImagingFactory->CreateFormatConverter(&FormatConverter);
			if(SUCCEEDED(result))
			{
				result=FormatConverter->Initialize((IWICBitmapSource*)FrameDecode,GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,NULL,0.0,WICBitmapPaletteTypeCustom);
				if(SUCCEEDED(result))
					result=draw->RenderTarget->CreateBitmapFromWicBitmap((IWICBitmapSource*)FormatConverter,NULL,&D2DBitmap);
				FormatConverter->Release();
			}
			FrameDecode->Release();
		}
		BitmapDecoder->Release();
	}
	return SUCCEEDED(result)?D2DBitmap:NULL;
}