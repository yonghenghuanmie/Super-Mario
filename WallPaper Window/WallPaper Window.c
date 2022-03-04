#include <stdbool.h>
#include <assert.h>
#include <tchar.h>
#include <process.h>
#include <windows.h>
#define Export
#include "WallPaper Window.h"

HINSTANCE hInstance;
HWND hWallPaper;
LRESULT (CALLBACK *test)(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK GetTargetWindow(HWND hwnd,LPARAM lParam)
{
	HWND child=FindWindowEx(hwnd,NULL,_T("SHELLDLL_DefView"),NULL);
	if(child)
	{
		*(HWND*)lParam=FindWindowEx(child,NULL,_T("SysListView32"),_T("FolderView"));
		return false;
	}
	return true;
}

void Dispatch(void* Parameter)
{
	WALLPAPERWINDOW *WallPaperWindow=(WALLPAPERWINDOW*)Parameter;
	HWND hwnd=NULL;
	EnumWindows(GetTargetWindow,(LPARAM)&hwnd);
	if(hwnd==NULL)
	{
		MessageBox(NULL,_T("failed to find window.\r\nClass Name:SysListView32\r\nWindow Name:FolderView"),_T("Error!"),MB_OK|MB_ICONHAND);
		SendMessage(hWallPaper,WM_DESTROY,0,0);
		return;
	}
	AttachThreadInput(GetCurrentThreadId(),GetWindowThreadProcessId(hwnd,NULL),true);
	HANDLE hTimer=CreateWaitableTimer(NULL,false,NULL);
	LARGE_INTEGER time;
	time.QuadPart=-10000*WallPaperWindow->TimeInterval;
	SetWaitableTimer(hTimer,&time,WallPaperWindow->TimeInterval,NULL,NULL,false);
	do
	{
		WaitForSingleObject(hTimer,INFINITE);
		unsigned char keyboard[256];
		GetKeyboardState(keyboard);
		if(keyboard[VK_ESCAPE]&0x80)
		{
			SendMessage(hWallPaper,WM_CLOSE,0,0);
			AttachThreadInput(GetCurrentThreadId(),GetWindowThreadProcessId(hwnd,NULL),false);
			CancelWaitableTimer(hTimer);
			break;
		}

		POINT point;
		GetCursorPos(&point);
		if(WallPaperWindow->Timer)
			if(!WallPaperWindow->Timer(&point,keyboard,WallPaperWindow->TimerParameter))
				if(WallPaperWindow->ErrorProcess)
					WallPaperWindow->ErrorProcess(WallPaperWindow->Timer,WallPaperWindow->ErrorProcessParameter);
	} while(true);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static WALLPAPERWINDOW *WallPaperWindow;
	switch(message)
	{
		case WM_CREATE:
			WallPaperWindow=(WALLPAPERWINDOW*)((CREATESTRUCT*)lParam)->lpCreateParams;
			if(WallPaperWindow->Initialize)
				if(!WallPaperWindow->Initialize(hwnd,WallPaperWindow->InitializeParameter))
					if(WallPaperWindow->ErrorProcess)
						WallPaperWindow->ErrorProcess(WallPaperWindow->Initialize,WallPaperWindow->ErrorProcessParameter);
			if(WallPaperWindow->TimeInterval)
				_beginthread(Dispatch,0,WallPaperWindow);
			return 0;

		case WM_PAINT:
			if(WallPaperWindow->Paint)
				if(!WallPaperWindow->Paint(WallPaperWindow->PaintParameter))
					if(WallPaperWindow->ErrorProcess)
						WallPaperWindow->ErrorProcess(WallPaperWindow->Paint,WallPaperWindow->ErrorProcessParameter);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

bool CreateWallpaperWindow(WALLPAPERWINDOW *WallPaperWindow)
{
	assert(WallPaperWindow);
	WNDCLASSEX WndClassEx;
	WndClassEx.cbSize			 = sizeof(WNDCLASSEX);
	WndClassEx.hbrBackground	 = GetStockObject(WHITE_BRUSH);
	WndClassEx.hCursor			 = (HCURSOR)LoadImage(NULL,(TCHAR*)IDC_ARROW,IMAGE_CURSOR,0,0,LR_SHARED);
	WndClassEx.hIconSm			 = NULL;
	WndClassEx.hIcon			 = (HICON)LoadImage(NULL,(TCHAR*)IDI_APPLICATION,IMAGE_ICON,0,0,LR_SHARED);
	WndClassEx.hInstance		 = hInstance;
	WndClassEx.lpfnWndProc		 = WndProc;
	WndClassEx.lpszClassName	 = TEXT("WallPaperWindow");
	WndClassEx.lpszMenuName		 = NULL;
	WndClassEx.style			 = CS_HREDRAW|CS_VREDRAW;
	WndClassEx.cbWndExtra		 = 0;
	WndClassEx.cbClsExtra		 = 0;
	RegisterClassEx(&WndClassEx);
	HWND hProgram=FindWindow(_T("Progman"),_T("Program Manager"));
	hWallPaper = CreateWindowEx(WS_EX_TOOLWINDOW,TEXT("WallPaperWindow"),TEXT("WallPaperWindow"),WS_CHILD/*WS_POPUP*/|WS_VISIBLE,
		0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),hProgram,0,hInstance,WallPaperWindow);
	assert(hWallPaper);
	MSG msg;
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if(WallPaperWindow->Close)
		if(!WallPaperWindow->Close(WallPaperWindow->CloseParameter))
			if(WallPaperWindow->ErrorProcess)
				WallPaperWindow->ErrorProcess(WallPaperWindow->Close,WallPaperWindow->ErrorProcessParameter);
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hInstance=hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}