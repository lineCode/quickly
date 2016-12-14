#pragma once
#include <windows.h>

typedef struct _PSWT_DATA
{
	HINSTANCE hInstance;
	HWND hWndParent;
	HWND hWndCurrent;
} PSWT_DATA, *LPPSWT_DATA;

DWORD WINAPI PngShadowWndThread(LPVOID lpData);