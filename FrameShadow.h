#pragma once

class CFrameShadow
{
public:

	CFrameShadow(HWND hParent, HINSTANCE hInst, int X, int Y, int Width, int Height);
	~CFrameShadow(void);
	
	
	void Moving(LPRECT pRect);
	void Move(int x, int y);
	void SetWindow(int Direct, HWND hWnd);
	void SetTransparent(int Value, BOOL bActivate);
	int  GetTransparent();

protected:
	
	HWND m_hWnd[4];
	HWND m_hWndParent;
	int m_iTransparent;
	RECT m_rcParent;
	
	HINSTANCE m_hInstance;
	
};