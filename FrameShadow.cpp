#include "StdAfx.h"
#include "resource.h"
#include "FrameShadow.h"

const TCHAR* ClassName = _T("QuicklyShadow");

enum { FS_BOTTOM=0, FS_TOP, FS_LEFT, FS_RIGHT, FS_LAST };

LRESULT CALLBACK CFrameShadowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef struct _SHADOWINFO
{
	HWND			hWndParent;
	HINSTANCE		hInst;
	DWORD			Direct;
	LPRECT			prcParent;
	Gdiplus::Image*	pImage1;
	Gdiplus::Image*	pImage2;
	Gdiplus::Image*	pImage3;
	CFrameShadow*	pObject;
	HDC				hdcMemory;
//	HBITMAP			hBitMap;
	PVOID			pvBits;
	HBITMAP			hbmpMem;
	HGDIOBJ			hbmpOld;
	_SHADOWINFO()
	{
		memset(this, 0, sizeof(*this));
	}
	
} SHADOWINFO, *LPSHADOWINFO;

void CalcDirectRect(LPRECT lpParent, int Direct, LPRECT lpTarget)
{
	switch( Direct )
	{
	case FS_BOTTOM:
		lpTarget->left	= lpParent->left-5;
		lpTarget->top	= lpParent->bottom;
		lpTarget->right	= lpParent->right+5;
		lpTarget->bottom = lpParent->bottom+7;
		break;
	case FS_TOP:
		lpTarget->left	= lpParent->left-5;
		lpTarget->top	= lpParent->top-4;
		lpTarget->right	= lpParent->right+5;
		lpTarget->bottom = lpParent->top;
		break;
	case FS_LEFT:
		lpTarget->left	= lpParent->left-5;
		lpTarget->top	= lpParent->top;
		lpTarget->right	= lpParent->left;
		lpTarget->bottom = lpParent->bottom;
		break;
	case FS_RIGHT:
		lpTarget->left	= lpParent->right;
		lpTarget->top	= lpParent->top;
		lpTarget->right	= lpParent->right+5;
		lpTarget->bottom = lpParent->bottom;
		break;
	}
}

DWORD WINAPI CreateBordowWindow(LPVOID  lpInfo)
{
	LPSHADOWINFO lpShadowInfo = (LPSHADOWINFO) lpInfo;

	RECT rc;
	CalcDirectRect(lpShadowInfo->prcParent, lpShadowInfo->Direct, &rc);

	Sleep(250);

	HWND hWnd = CreateWindowEx(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED
		, ClassName
		, _T("")
		, WS_VISIBLE | WS_OVERLAPPED //| WS_POPUPWINDOW | WS_CLIPSIBLINGS
		, rc.left
		, rc.top
		, rc.right-rc.left
		, rc.bottom-rc.top
		, lpShadowInfo->hWndParent
		, NULL
		, lpShadowInfo->hInst
		, (LPVOID) lpShadowInfo
		);

	if ( hWnd != NULL )
	{
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		lpShadowInfo->pObject->SetWindow(lpShadowInfo->Direct, hWnd);
		InvalidateRect(hWnd, NULL, FALSE);
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

CFrameShadow::CFrameShadow(HWND hParent, HINSTANCE hInst, int X, int Y, int Width, int Height)
{
	m_hWndParent = hParent;
	m_hInstance = hInst;
	m_iTransparent = 255;

	m_hWnd[0] = NULL;
	m_hWnd[1] = NULL;
	m_hWnd[2] = NULL;
	m_hWnd[3] = NULL;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CFrameShadowWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= ClassName;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	m_rcParent.left = X;
	m_rcParent.top = Y;
	m_rcParent.right = X + Width;
	m_rcParent.bottom = Y + Height;


//	for(DWORD dwDirect=FS_BOTTOM; dwDirect<FS_LAST; dwDirect++)
	{
		LPSHADOWINFO lpSI = new SHADOWINFO;
		lpSI->Direct = FS_BOTTOM;// dwDirect;
		lpSI->hInst = hInst;
		lpSI->hWndParent = hParent;
		lpSI->prcParent = &m_rcParent;
		lpSI->pObject = this;
		CreateThread(NULL, 0, CreateBordowWindow, (LPVOID)lpSI, NULL, NULL);
	}
	{
		LPSHADOWINFO lpSI = new SHADOWINFO;
		lpSI->Direct = FS_TOP;// dwDirect;
		lpSI->hInst = hInst;
		lpSI->hWndParent = hParent;
		lpSI->prcParent = &m_rcParent;
		lpSI->pObject = this;
		CreateThread(NULL, 0, CreateBordowWindow, (LPVOID)lpSI, NULL, NULL);
	}
	{
		LPSHADOWINFO lpSI = new SHADOWINFO;
		lpSI->Direct = FS_LEFT;// dwDirect;
		lpSI->hInst = hInst;
		lpSI->hWndParent = hParent;
		lpSI->prcParent = &m_rcParent;
		lpSI->pObject = this;
		CreateThread(NULL, 0, CreateBordowWindow, (LPVOID)lpSI, NULL, NULL);
	}
	{
		LPSHADOWINFO lpSI = new SHADOWINFO;
		lpSI->Direct = FS_RIGHT;// dwDirect;
		lpSI->hInst = hInst;
		lpSI->hWndParent = hParent;
		lpSI->prcParent = &m_rcParent;
		lpSI->pObject = this;
		CreateThread(NULL, 0, CreateBordowWindow, (LPVOID)lpSI, NULL, NULL);
	}
}

void CFrameShadow::SetWindow(int Direct, HWND hWnd)
{
	m_hWnd[Direct] = hWnd;
}

void CFrameShadow::SetTransparent(int Value, BOOL bActivate)
{
	return ;
	// ::SetLayeredWindowAttributes(m_hWnd[0], 0, Value, LWA_ALPHA);
	m_iTransparent = Value;
	for(int i=FS_BOTTOM; i<FS_LAST; i++)
	{
//		SendMessage(m_hWnd[i], WM_NCACTIVATE, bActivate, GetCurrentThreadId());
//		SendMessage(m_hWnd[i], WM_ACTIVATE, bActivate , GetCurrentThreadId());
//		SendMessage(m_hWnd[i], WM_ACTIVATEAPP, bActivate, GetCurrentThreadId());
//		InvalidateRect(m_hWnd[i], NULL, FALSE);
		SetWindowPos(m_hWnd[i], Setting.hCurrentDialog, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}

int CFrameShadow::GetTransparent()
{
	return m_iTransparent;
}

void CFrameShadow::Moving(LPRECT pRect)
{
	RECT rc;
	memcpy(&m_rcParent, pRect, sizeof(RECT));
	for(int i=FS_RIGHT;i>=FS_BOTTOM;i--)
	{
		CalcDirectRect(pRect, i, &rc);
		//SetForegroundWindow(m_hWnd[i]);
		//::MoveWindow(m_hWnd[i], rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
		::SetWindowPos( m_hWnd[i]
			, NULL
			, rc.left
			, rc.top
			, 0
			, 0
			, SWP_NOZORDER | SWP_NOSIZE
			);

// 		WINDOWPOS Pos;
// 		memset(&Pos, 0, sizeof(Pos));
// 		Pos.hwnd = m_hWnd[i];
// 		Pos.hwndInsertAfter = NULL;
// 		Pos.x = rc.left;
// 		Pos.y = rc.top;
// 		Pos.cx = 0;
// 		Pos.cy = 0;
// 		Pos.flags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
// 		::SendMessage( m_hWnd[i], WM_WINDOWPOSCHANGING, NULL, (LPARAM)&Pos );
// 		Pos.cx = rc.right-rc.left;
// 		Pos.cy = rc.bottom-rc.top;
// 		Pos.flags |= 0x0800;
// 		::SendMessage( m_hWnd[i], WM_WINDOWPOSCHANGED, NULL, (LPARAM)&Pos);
// 		::SendMessage( m_hWnd[i], WM_MOVE, NULL, MAKELPARAM(rc.left, rc.top) );
	}
}

void CFrameShadow::Move(int x, int y)
{
// 	RECT orc;
// 	::GetWindowRect(m_hWndParent, &orc);
// 	for(int i=FS_BOTTOM;i<FS_LAST;i++)
// 	{
// 		RECT rc;
// 		CalcDirectRect(&orc, i, &rc);
// 		WINDOWPOS Pos;
// 		memset(&Pos, 0, sizeof(Pos));
// 		Pos.hwnd = m_hWnd[i];
// 		Pos.hwndInsertAfter = NULL;
// 		Pos.x = rc.left;
// 		Pos.y = rc.top;
// 		Pos.cx = 0;
// 		Pos.cy = 0;
// 		Pos.flags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
// 		::SendMessage( m_hWnd[i], WM_WINDOWPOSCHANGING, NULL, (LPARAM)&Pos );
// 		Pos.cx = rc.right-rc.left;
// 		Pos.cy = rc.bottom-rc.top;
// 		Pos.flags |= 0x0800;
// 		::SendMessage( m_hWnd[i], WM_WINDOWPOSCHANGED, NULL, (LPARAM)&Pos);
// 		::SendMessage( m_hWnd[i], WM_MOVE, NULL, MAKELPARAM(rc.left, rc.top) );
// 	}
}

CFrameShadow::~CFrameShadow(void)
{

	for(int i=FS_BOTTOM;i<FS_LAST;i++)
	{
		HWND hTarget = m_hWnd[i];
		PostMessage(hTarget, WM_CLOSE, 0, 0);
	}

	
}

Image* MakeImageStream(HINSTANCE hInst, UINT uID )
{
	HRSRC hRsrc = NULL;
	DWORD dwSize = 0;
	LPBYTE lpRsrc = NULL;
	LPBYTE pMem = NULL;
	HGLOBAL hMem = NULL;
	Image* pImage = NULL;
	IStream* pStream = NULL;

	hRsrc = ::FindResource ( hInst, MAKEINTRESOURCE(uID), L"PNG");
	if ( hRsrc != NULL )
	{
		dwSize = ::SizeofResource( hInst, hRsrc);
		lpRsrc = (LPBYTE)::LoadResource( hInst, hRsrc);
		if ( lpRsrc != NULL )
		{
			hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
			if ( hMem != NULL )
			{
				pMem = (LPBYTE)::GlobalLock(hMem);
				memcpy( pMem, lpRsrc, dwSize);
				CreateStreamOnHGlobal( hMem, FALSE, &pStream);
				pImage = Image::FromStream(pStream);
				//pImage->RotateFlip(Rotate90FlipY);
				pStream->Release();
				::GlobalUnlock(hMem);
			}
			::FreeResource(lpRsrc);
		}
	}

	return pImage;
}



LRESULT CALLBACK CFrameShadowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	LPSHADOWINFO pData;
	
	//TRACE("%x\n", message);

	switch(message)
	{
	case WM_ACTIVATEAPP:
		{
			RECT rcTarget;
#ifdef _M_X64
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWLP_USERDATA );
#else 
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWL_USERDATA);
#endif
			CalcDirectRect(pData->prcParent, pData->Direct, &rcTarget);		

// 			SIZE sizeWindow = { rcTarget.right-rcTarget.left, rcTarget.bottom-rcTarget.top };
// 			POINT ptWinPos = { rcTarget.left, rcTarget.top };

			if ( wParam )
			{
// 				::SetWindowPos(
// 					hWnd
// 					, HWND_TOPMOST
// 					, rcTarget.left
// 					, rcTarget.top
// 					, rcTarget.right-rcTarget.left
// 					, rcTarget.bottom-rcTarget.top
// 					, SWP_SHOWWINDOW);
			}
			else
			{
// 				::SetWindowPos(
// 					hWnd
// 					, HWND_BOTTOM
// 					, rcTarget.left
// 					, rcTarget.top
// 					, rcTarget.right-rcTarget.left
// 					, rcTarget.bottom-rcTarget.top
// 					, SWP_SHOWWINDOW);
			}

// 			::SetWindowPos(
// 				hWnd
// 				, HWND_NOTOPMOST
// 				, rcTarget.left
// 				, rcTarget.top
// 				, rcTarget.right-rcTarget.left
// 				, rcTarget.bottom-rcTarget.top
// 				, SWP_SHOWWINDOW);
		}
		break;
	case WM_CREATE:
		{
			LPCREATESTRUCT lpCS = (LPCREATESTRUCT) lParam;
#ifdef _M_X64
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) lpCS->lpCreateParams );
#else
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR) lpCS->lpCreateParams );
#endif
			pData = (LPSHADOWINFO)lpCS->lpCreateParams;
			switch( pData->Direct )
			{
			case FS_BOTTOM:
				pData->pImage1 = MakeImageStream(pData->hInst, IDB_BOTTOM_LEFT);
				pData->pImage2 = MakeImageStream(pData->hInst, IDB_BOTTOM_CENTER);
				pData->pImage3 = MakeImageStream(pData->hInst, IDB_BOTTOM_RIGHT);
				break;
			case FS_TOP:
				pData->pImage1 = MakeImageStream(pData->hInst, IDB_TOP_LEFT);
				pData->pImage2 = MakeImageStream(pData->hInst, IDB_TOP_CENTER);
				pData->pImage3 = MakeImageStream(pData->hInst, IDB_TOP_RIGHT);
				break;
			case FS_LEFT:
				pData->pImage1 = MakeImageStream(pData->hInst, IDB_LEFT_TOP);
				pData->pImage2 = MakeImageStream(pData->hInst, IDB_LEFT_LINE);
				break;
			case FS_RIGHT:
				pData->pImage1 = MakeImageStream(pData->hInst, IDB_RIGHT_TOP);
				pData->pImage2 = MakeImageStream(pData->hInst, IDB_RIGHT_LINE);
				break;
			}

			DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
			if ((dwExStyle & 0x80000) != 0x80000)
				::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

			InvalidateRect(hWnd, NULL, TRUE);
			
			break;
		}
	case WM_PAINT:
		{
			RECT rcTarget;
			hdc = BeginPaint(hWnd, &ps);
// 			{
// 				RECT rc;
// 				::GetClientRect(hWnd, &rc);
// 				::SetBkColor(hdc, RGB(255,255,0));
// 				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
// 				EndPaint(hWnd, &ps);
// 				break;
// 			}

#ifdef _M_X64
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWLP_USERDATA );
#else
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWL_USERDATA );
#endif
			CalcDirectRect(pData->prcParent, pData->Direct, &rcTarget);		

			SIZE sizeWindow = { rcTarget.right-rcTarget.left, rcTarget.bottom-rcTarget.top };
			POINT ptWinPos = { rcTarget.left, rcTarget.top };


			
			CalcDirectRect(pData->prcParent, pData->Direct, &rcTarget);	
			
// 			::SetWindowPos(
// 				hWnd
// 				, HWND_TOPMOST
// 				, rcTarget.left
// 				, rcTarget.top
// 				, rcTarget.right-rcTarget.left
// 				, rcTarget.bottom-rcTarget.top
// 				, SWP_SHOWWINDOW);
// 
// 			::SetWindowPos(
// 				hWnd
// 				, HWND_NOTOPMOST
// 				, rcTarget.left
// 				, rcTarget.top
// 				, rcTarget.right-rcTarget.left
// 				, rcTarget.bottom-rcTarget.top
// 				, SWP_SHOWWINDOW);

			if ( pData->hdcMemory == NULL )
			{
				pData->hdcMemory = ::CreateCompatibleDC(hdc);
//				pData->hBitMap = CreateCompatibleBitmap(hdc, sizeWindow.cx, sizeWindow.cy);
//				::SelectObject(pData->hdcMemory, pData->hBitMap);

				BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
				int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
				stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
				stBmpInfoHeader.biWidth = sizeWindow.cx;   
				stBmpInfoHeader.biHeight = sizeWindow.cy;   
				stBmpInfoHeader.biPlanes = 1;   
				stBmpInfoHeader.biBitCount = 32;   
				stBmpInfoHeader.biCompression = BI_RGB;   
				stBmpInfoHeader.biClrUsed = 0;   
				stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   


				pData->pvBits = NULL;   
				pData->hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pData->pvBits, NULL, 0);
				
				memset( pData->pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);

				if( pData->hbmpMem )
				{   
					pData->hbmpOld = ::SelectObject( pData->hdcMemory, pData->hbmpMem);

					RECT rct;
					rct.left = rct.top = 0;
					rct.right = sizeWindow.cx;
					rct.bottom = sizeWindow.cy;
					::SetBkColor(pData->hdcMemory, RGB(0,0,0));
					::ExtTextOut(pData->hdcMemory, 0, 0, ETO_OPAQUE, &rct, NULL, 0, NULL);

					Gdiplus::Graphics graph(pData->hdcMemory);
					graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

					switch( pData->Direct )
					{
					case FS_BOTTOM:
						for(int i=10;i<sizeWindow.cx-10;i++)
						{
							graph.DrawImage(pData->pImage2, i, 0, 1, sizeWindow.cy );
						}
						graph.DrawImage(pData->pImage1, 0, 0, 10, sizeWindow.cy );
						graph.DrawImage(pData->pImage3, sizeWindow.cx-10, 0, 10, sizeWindow.cy );
						break;
					case FS_TOP:
						for(int i=10;i<sizeWindow.cx-10;i++)
						{
							graph.DrawImage(pData->pImage2, i, 0, 1, sizeWindow.cy );
						}
						graph.DrawImage(pData->pImage1, 0, 0, 10, sizeWindow.cy );
						graph.DrawImage(pData->pImage3, sizeWindow.cx-10, 0, 10, sizeWindow.cy );
						break;
					case FS_LEFT:
						graph.DrawImage(pData->pImage1, 0, 0, 5, 5 );
						for(int i=5;i<sizeWindow.cy;i++)
						{
							graph.DrawImage(pData->pImage2, 0, i, 5, 1 );
						}
						break;
					case FS_RIGHT:
						graph.DrawImage(pData->pImage1, 0, 0, 5, 5 );
						for(int i=5;i<sizeWindow.cy;i++)
						{
							graph.DrawImage(pData->pImage2, 0, i, 5, 1 );
						}
						break;
					}

					graph.ReleaseHDC(pData->hdcMemory);
				}
			}

			POINT ptSrc = { 0, 0 };

			BLENDFUNCTION m_blend;
			m_blend.BlendOp = 0;
			m_blend.BlendFlags = 0;
			m_blend.AlphaFormat = 1; 
			m_blend.SourceConstantAlpha = 255;// pData->pObject->GetTransparent();

			// 				HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
			// 				typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
			// 				MYFUNC UpdateLayeredWindow;
			// 				UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");

			UpdateLayeredWindow(hWnd, hdc, &ptWinPos, &sizeWindow, pData->hdcMemory, &ptSrc, 125, &m_blend, ULW_ALPHA);
			//::DeleteDC(hdc);  <-- ¿Ö ³ÖÁö?

			EndPaint(hWnd, &ps);
		}
		break;
// 	case WM_WINDOWPOSCHANGING:
// 	case WM_WINDOWPOSCHANGED:
// 	case WM_MOVE:
// 		return 0;
// 		break;
	case WM_ERASEBKGND:
		return 0;

	case WM_DESTROY:
		break;
		{
#ifdef _M_X64
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWLP_USERDATA );
#else
			pData = (LPSHADOWINFO) GetWindowLongPtr(hWnd, GWL_USERDATA );
#endif

			if ( pData->pImage1 != NULL )
				delete pData->pImage1;

			if ( pData->pImage2 != NULL )
				delete pData->pImage2;

			if ( pData->pImage3 != NULL )
				delete pData->pImage3;

			::SelectObject(pData->hdcMemory, pData->hbmpOld);   
//			::DeleteObject(pData->hBitMap);
			::DeleteObject(pData->hbmpMem);
			::DeleteDC(pData->hdcMemory);

			delete pData;
		}

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}