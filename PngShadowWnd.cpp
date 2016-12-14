// PngWnd.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "resource.h"
#include "PngShadowWnd.h"
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define BOTTOM_HEIGHT	7
#define TOP_HEIGHT		5
#define LEFT_WIDTH		5
#define RIGHT_WIDTH		5

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
Gdiplus::Image*		GetImageStream(HINSTANCE hInstance, UINT uID);

HINSTANCE		hParentInstance;
int				iParentWidth, iParentHeight;
Gdiplus::Image* pImagesPngRes[12]	= { NULL, };
const TCHAR*	szTitle				= _T("PngShadowWnd");
const TCHAR*	szWindowClass		= _T("PngShadowWnd");
WNDPROC			lpOldAgentNameProc	= NULL;
HWND			hPndShadowWnd		= NULL;
HWND			hParentWnd			= NULL;

Gdiplus::Image* GetImageStream(HINSTANCE hInstance, UINT uID )
{
	Gdiplus::Image* pImage = NULL;

	HRSRC hRsrc = ::FindResource ( hInstance, MAKEINTRESOURCE(uID), L"PNG"); 
	DWORD dwSize = ::SizeofResource( hInstance, hRsrc);
	LPBYTE lpRsrc = (LPBYTE)::LoadResource( hInstance, hRsrc);
	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
	LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
	memcpy( pMem, lpRsrc, dwSize);
	IStream * pStream = NULL;
	::CreateStreamOnHGlobal( hMem, TRUE, &pStream); // FALSE 였음.
	pImage = Image::FromStream(pStream);
	::GlobalUnlock(hMem);
	pStream->Release();
	::FreeResource(lpRsrc);
	
	return pImage;
}


LRESULT FAR PASCAL ParentProc(HWND hWnd, WORD wMessage, WORD wParam, LONG lParam)
{
	//TRACE("%d\n", wMessage);
	switch (wMessage)
	{
	case WM_WINDOWPOSCHANGED:
	case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
			if ( lpWndPos->flags&SWP_NOMOVE ) 
			{
				::SetWindowPos( hPndShadowWnd
					, hParentWnd
					, 0//lpWndPos->x-LEFT_WIDTH
					, 0//lpWndPos->y-TOP_HEIGHT
					, 0
					, 0
					, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
			}
			else
			{
				::SetWindowPos( hPndShadowWnd
					, hParentWnd
					, lpWndPos->x-LEFT_WIDTH
					, lpWndPos->y-TOP_HEIGHT
					, 0
					, 0
					, SWP_NOSIZE|SWP_NOACTIVATE);
			}
		}
	default:
		break;

	}

	return CallWindowProc(lpOldAgentNameProc, hWnd, wMessage, wParam, lParam);;
}

DWORD WINAPI PngShadowWndThread(LPVOID lpData)
{
	LPPSWT_DATA pData = (LPPSWT_DATA) lpData;

	if ( pData == NULL ) return NULL;

	hParentWnd = pData->hWndParent;

	MSG msg;
	WNDCLASSEX wcex;
	ULONG_PTR gdiplusToken;
	RECT rcParent;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	
	for ( UINT uiPng = IDB_BOTTOM_LEFT; uiPng <= IDB_RIGHT_TOP; uiPng ++ )
	{
		UINT index = uiPng - IDB_BOTTOM_LEFT;
		pImagesPngRes[index] = GetImageStream(pData->hInstance, uiPng);
	}

	
	::GetWindowRect(pData->hWndParent, &rcParent);
	iParentWidth = rcParent.right-rcParent.left + LEFT_WIDTH + RIGHT_WIDTH;
	iParentHeight = rcParent.bottom-rcParent.top + BOTTOM_HEIGHT + TOP_HEIGHT;
	lpOldAgentNameProc = (WNDPROC) SetWindowLongPtr ( pData->hWndParent
		, DWLP_DLGPROC // GWLP_WNDPROC
		, (LONG_PTR) ParentProc );
		
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= pData->hInstance;
	wcex.hIcon			= NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PNGWND));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_PNGWND);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&wcex);

// 	hPndShadowWnd = CreateWindow( szWindowClass
// 		, szTitle
// 		, WS_POPUP | WS_DISABLED | WS_CHILD
// 		, rcParent.left - LEFT_WIDTH
// 		, rcParent.top - TOP_HEIGHT
// 		, iParentWidth
// 		, iParentHeight
// 		, NULL
// 		, NULL
// 		, pData->hInstance
// 		, NULL);

	hPndShadowWnd = CreateWindowEx( WS_EX_TOOLWINDOW
		, szWindowClass
		, szTitle
		, WS_POPUP | WS_DISABLED 
		, rcParent.left - LEFT_WIDTH
		, rcParent.top - TOP_HEIGHT
		, iParentWidth
		, iParentHeight
		, NULL
		, NULL
		, pData->hInstance
		, NULL);

	pData->hWndCurrent = hPndShadowWnd;

	ShowWindow(hPndShadowWnd, SW_SHOW);
	UpdateWindow(hPndShadowWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	for(int i=0; i<9; i++)
	{
		if ( pImagesPngRes[i] != NULL )
			DllExports::GdipFree(pImagesPngRes[i]);
	}
	
	Gdiplus::GdiplusShutdown(gdiplusToken);

	delete pData;

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		{
			RECT rcWindow = { 0, 0, iParentWidth, iParentHeight};
			::AdjustWindowRect(&rcWindow, (DWORD)GetWindowLong(hWnd, GWL_STYLE), FALSE);

			DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
			
			if ((dwExStyle & 0x80000) != 0x80000)
				::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle|0x80000|WS_EX_TOOLWINDOW);

			BLENDFUNCTION m_blend;
			m_blend.BlendOp			= 0;
			m_blend.BlendFlags		= 0;
			m_blend.AlphaFormat		= 1;
			m_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA
		
			GetWindowRect(hWnd, &rcWindow);

			SIZE sizeWindow = { rcWindow.right-rcWindow.left, rcWindow.bottom-rcWindow.top };

			hdc = BeginPaint(hWnd, &ps);
			HDC hdcMemory = ::CreateCompatibleDC(hdc);

			BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
			int nBytesPerLine			= ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
			stBmpInfoHeader.biSize		= sizeof(BITMAPINFOHEADER);   
			stBmpInfoHeader.biWidth		= sizeWindow.cx;   
			stBmpInfoHeader.biHeight	= sizeWindow.cy;   
			stBmpInfoHeader.biPlanes	= 1;   
			stBmpInfoHeader.biBitCount	= 32;   
			stBmpInfoHeader.biCompression = BI_RGB;   
			stBmpInfoHeader.biClrUsed	= 0;   
			stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

			PVOID pvBits = NULL;   
			HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);

			memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
			if(hbmpMem)   
			{   
				HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);
				POINT ptWinPos = { rcWindow.left, rcWindow.top };
				Gdiplus::Graphics graph(hdcMemory);
				graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

				{
					// BOTTOM
					graph.DrawImage(pImagesPngRes[0], 0, sizeWindow.cy-BOTTOM_HEIGHT, 10, BOTTOM_HEIGHT );
					for(int i=10;i<sizeWindow.cx-10;i++)
					{
						graph.DrawImage(pImagesPngRes[1], i, sizeWindow.cy-BOTTOM_HEIGHT, 1, BOTTOM_HEIGHT );
					}
					graph.DrawImage(pImagesPngRes[2], sizeWindow.cx-10, sizeWindow.cy-BOTTOM_HEIGHT, 10, BOTTOM_HEIGHT );


					// TOP 
					graph.DrawImage(pImagesPngRes[3], 0, 0, 10, TOP_HEIGHT );
					for(int i=10;i<sizeWindow.cx-10;i++)
					{
						graph.DrawImage(pImagesPngRes[4], i, 0, 1, TOP_HEIGHT );
					}
					graph.DrawImage(pImagesPngRes[5], sizeWindow.cx-10, 0, 10, TOP_HEIGHT );

					// LEFT
					graph.DrawImage(pImagesPngRes[8], 0, TOP_HEIGHT, 5, 5);
					for(int i=TOP_HEIGHT+5;i<sizeWindow.cy-BOTTOM_HEIGHT;i++)
					{
						graph.DrawImage(pImagesPngRes[7], 0, i, 5, 1 );
					}
					// RIGHT
					graph.DrawImage(pImagesPngRes[11], sizeWindow.cx-5, TOP_HEIGHT, 5, 5 );
					for(int i=TOP_HEIGHT+5;i<sizeWindow.cy-BOTTOM_HEIGHT;i++)
					{
						graph.DrawImage(pImagesPngRes[10], sizeWindow.cx-5, i, 5, 1 );
					}
				}

				HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
				typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
				MYFUNC UpdateLayeredWindow;
				UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
				POINT ptSrc = { 0, 0};
				UpdateLayeredWindow(hWnd, hdc, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &m_blend, 2);

				graph.ReleaseHDC(hdcMemory);
				::SelectObject( hdcMemory, hbmpOld);   
				::DeleteObject(hbmpMem); 
			}

			::DeleteDC(hdcMemory);
			::DeleteDC(hdc);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_NCHITTEST:
		if ( DefWindowProc(hWnd, message, wParam, lParam) == HTCLIENT )
			return HTCAPTION;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}