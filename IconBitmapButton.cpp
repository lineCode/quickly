// IconBitmapButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "IconBitmapButton.h"
#include <strsafe.h>

#define WHITE RGB(0xff,0xff,0xff)
#define BLACK RGB(0x00,0x00,0x00)

#define FOCUS_H RGB( 85, 120, 157)
#define FOCUS_L RGB(108, 178, 253)

IMPLEMENT_DYNAMIC(CIconBitmapButton, CBitmapButton)

DWORD CIconBitmapButton::GetFocusHColor()
{
	DWORD dwFocusColor = FOCUS_H;
	
	int FR = (int)GetRValue(dwFocusColor);
	int PR = GetRValue(m_crParent);
	int RR = (FR-PR)/2;
	RR = RR + PR;

	int FG = (int)GetGValue(dwFocusColor);
	int PG = GetRValue(m_crParent);
	int RG = (FG-PG)/2;
	RG = RG + PG;

	int FB = (int)GetBValue(dwFocusColor);
	int PB = GetRValue(m_crParent);
	int RB = (FB-PB)/2;
	RB = RB + PB;

	return RGB(RR, RG, RB);
}

DWORD CIconBitmapButton::ModClr(int R, int G, int B, int BaseColor)
{
	int BR = (int)GetRValue(BaseColor);
	int BG = (int)GetGValue(BaseColor);
	int BB = (int)GetBValue(BaseColor);
	
	int Red = BR + R;
	int Green = BG + G;
	int Blue = BB + B;

	if ( Red < 0 ) Red = 0;
	if ( Red > 255 ) Red = 255;
	if ( Green < 0 ) Green = 0;
	if ( Green > 255 ) Green = 255;
	if ( Blue < 0 ) Blue = 0;
	if ( Blue > 255 ) Blue = 255;

	return RGB(Red, Green, Blue);
}


CIconBitmapButton::CIconBitmapButton()
{
	m_bTracking = FALSE;
	m_hOldCursor = NULL;
	m_pToolTip = NULL;
	{
		CFont f;
		f.CreateStockObject(DEFAULT_GUI_FONT);
		LOGFONT lf;
		f.GetLogFont(&lf);
		StringCchCopy(lf.lfFaceName, 32, _T("궁서"));
		lf.lfWeight = 700;
		lf.lfHeight = 20;
		lf.lfQuality = 6;
		m_fTitle.CreateFontIndirect(&lf);

		StringCchCopy(lf.lfFaceName, 32, _T("돋음"));
		lf.lfWeight = 400;
		lf.lfHeight = 12;
		lf.lfQuality = 0;
		m_fDescription.CreateFontIndirect(&lf);
	}



}

CIconBitmapButton::~CIconBitmapButton()
{
	if ( m_pToolTip != NULL )
	{
		delete m_pToolTip;
	}
}


BEGIN_MESSAGE_MAP(CIconBitmapButton, CBitmapButton)
	ON_WM_DRAWITEM()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CIconBitmapButton::SetDescription(CString Text)
{
	m_sDescription = Text;
	m_pToolTip = new CToolTipCtrl;
	if ( m_pToolTip->Create(this, TTS_BALLOON|WS_POPUP|WS_EX_TOOLWINDOW) )
	{
		m_pToolTip->SetTitle(4, m_sTitle);
		m_pToolTip->AddTool(this, m_sDescription);
//		m_pToolTip->SetWindowTheme(_T("explorer"));
		m_pToolTip->Activate(TRUE);
	}
	else
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

void CIconBitmapButton::SetTitle(CString Text)
{
	m_sTitle = Text;
}

void CIconBitmapButton::SetParentBkColor(DWORD dwColor)
{
	m_crParent = dwColor;
}

void CIconBitmapButton::SetButtonBkColor(DWORD dwColor)
{
	m_crButton = dwColor;
}

void CIconBitmapButton::SetFontColor(DWORD dwColor)
{
	m_ftButton = dwColor;
}


static void FillColor(TRIVERTEX* pVertices, void* pMesh, CRect& rc, DWORD dwUpColor, DWORD dwDownColor)
{
	GRADIENT_RECT* pRect = reinterpret_cast<GRADIENT_RECT*>(pMesh);
	pVertices[0].x      = rc.left;
	pVertices[0].y      = rc.top;
	pVertices[0].Red    = GetRValue(dwUpColor)*0x100;
	pVertices[0].Green  = GetGValue(dwUpColor)*0x100;
	pVertices[0].Blue   = GetBValue(dwUpColor)*0x100;
	pVertices[0].Alpha  = 0x0000;
	pVertices[1].x      = rc.right;
	pVertices[1].y      = rc.bottom;
	pVertices[1].Red    = GetRValue(dwDownColor)*0x100;
	pVertices[1].Green  = GetGValue(dwDownColor)*0x100;
	pVertices[1].Blue   = GetBValue(dwDownColor)*0x100;
	pVertices[1].Alpha  = 0x0000;
	pRect->UpperLeft  = 0;
	pRect->LowerRight = 1;
}

void CIconBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CRect R;
	R.CopyRect(&lpDIS->rcItem);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	TRIVERTEX Vert[2];
	GRADIENT_RECT GRect;

	BOOL bIsFocused = lpDIS->itemState & ODS_FOCUS;
	//rcClient.DeflateRect(1, 1);
	//pDC->FillSolidRect(&rcClient, RGB(0x6c, 0xb2, 0xfd));

	if ( m_bTracking )
	{
		if ( lpDIS->itemState & ODS_SELECTED )
		{
			if ( bIsFocused )
			{
				pDC->FillSolidRect(&R, GetFocusHColor());
				R.DeflateRect(1,1);
				pDC->FillSolidRect(&R, FOCUS_L);
			}	
			else
			{
				pDC->FillSolidRect(&R, m_crParent);
				R.DeflateRect(1,1);
				FillColor(Vert, &GRect, R, ModClr(6,6,6,m_crParent), ModClr(17,17,17,m_crParent)); //// 85 62 ( RGB(68,68,68), RGB(79,79,79));
				pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
			}

			R.DeflateRect(1,1);
			pDC->FillSolidRect(&R, ModClr(-73,-73,-73, m_crButton));// RGB(12,12,12));
			R.DeflateRect(1,1);
			pDC->FillSolidRect(&R, ModClr(-38,-38,-38, m_crButton));// RGB(47,47,47));
			R.top++;
			pDC->FillSolidRect(&R, ModClr(-28,-28,-28, m_crButton));// RGB(57,57,57));
			R.top++;
			FillColor(Vert, &GRect, R, ModClr(-30,-30,-30, m_crButton), ModClr(-38,-38,-38,m_crButton));//  RGB(55,55,55), RGB(47,47,47));
			pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
			R.left++;
			R.right--;
			R.top++;
			FillColor(Vert, &GRect, R, ModClr(-24,-24,-24,m_crButton), ModClr(-33,-33,-33,m_crButton));// RGB(61,61,61), RGB(52,52,52));
			pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
		}
		else
		{
			if ( bIsFocused )
			{
				pDC->FillSolidRect(&R, GetFocusHColor());
				R.DeflateRect(1,1);
				pDC->FillSolidRect(&R, FOCUS_L);
			}	
			else
			{	
				pDC->FillSolidRect(&R, m_crParent);
				R.DeflateRect(1,1);
				FillColor(Vert, &GRect, R, ModClr(6,6,6,m_crParent), ModClr(17,17,17,m_crParent));
				pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
			}
			R.DeflateRect(1,1);
			pDC->FillSolidRect(&R, ModClr(-73,-73,-73, m_crButton));// RGB(12,12,12));
			R.DeflateRect(1,1);
			FillColor(Vert, &GRect, R, ModClr(50,50,50, m_crButton), ModClr(4,4,4,m_crButton));// RGB(135,135,135), RGB(89,89,89));
			pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
			R.DeflateRect(1,1);
			FillColor(Vert, &GRect, R, ModClr(37,37,37,m_crButton), ModClr(0,0,0,m_crButton)); // RGB(122,122,122), RGB(85,85,85));
			pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
		}
	}
	else
	{
		if ( bIsFocused )
		{
			pDC->FillSolidRect(&R, GetFocusHColor());
			R.DeflateRect(1,1);
			pDC->FillSolidRect(&R, FOCUS_L);
		}	
		else
		{	
			pDC->FillSolidRect(&R, m_crParent);
			R.DeflateRect(1,1);
			FillColor(Vert, &GRect, R, ModClr(6,6,6,m_crParent), ModClr(17,17,17,m_crParent));
			pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
		}

		R.DeflateRect(1,1);
		pDC->FillSolidRect(&R, ModClr(-73,-73,-73, m_crButton)); //RGB(12,12,12));
		R.DeflateRect(1,1);
		FillColor(Vert, &GRect, R, ModClr(25,25,25, m_crButton), ModClr(-7,-7,-7,m_crButton));//  RGB(110,110,110), RGB(78,78,78));
		pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);
		R.DeflateRect(1,1);
		FillColor(Vert, &GRect, R, ModClr(11,11,11,m_crButton), ModClr(-11,-11,-11,m_crButton));//  RGB(96,96,96), RGB(74,74,74));
		pDC->GradientFill(Vert, 2, &GRect, 1, GRADIENT_FILL_RECT_V);

	}


	pDC->SetBkMode(TRANSPARENT);
	CFont* pOldFont = pDC->SelectObject(&m_fTitle);
	CRect rcFont;
	rcFont.CopyRect(&lpDIS->rcItem);
	
	pDC->DrawText(m_sTitle, &rcFont, DT_SINGLELINE | DT_CALCRECT );

	int W = rcFont.Width();
	int H = rcFont.Height();
	int X = (lpDIS->rcItem.right - W)/2;
	int Y = (lpDIS->rcItem.bottom - H)/2;

	rcFont.left = X;
	rcFont.top = Y;
	rcFont.right = X + W;
	rcFont.bottom = Y + H;

	if ( m_bTracking && lpDIS->itemState & ODS_SELECTED )
	{
		rcFont.MoveToXY(rcFont.left+1, rcFont.top+1);
	}

	rcFont.MoveToXY(rcFont.left+1, rcFont.top+1);
	pDC->SetTextColor(RGB(0,0,0));
	//pDC->DrawText(m_sTitle, &rcFont, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	
	pDC->TextOut(rcFont.left, rcFont.top, m_sTitle);

	rcFont.MoveToXY(rcFont.left-1, rcFont.top-1);
	pDC->SetTextColor(RGB(255,255,255));
	//pDC->DrawText(m_sTitle, &rcFont, DT_LEFT | DT_VCENTER | DT_SINGLELINE   );
	pDC->SetTextColor(m_ftButton);
	pDC->TextOut(rcFont.left, rcFont.top, m_sTitle);
	pDC->SelectObject(pOldFont);

	return;
}

void CIconBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}
void CIconBitmapButton::OnMouseLeave()
{
	m_bTracking = FALSE;
	Invalidate(FALSE);
	CBitmapButton::OnMouseLeave();
}

void CIconBitmapButton::OnMouseHover(UINT nFlags, CPoint point)
{
	Invalidate(FALSE);

	CBitmapButton::OnMouseHover(nFlags, point);
}
BOOL CIconBitmapButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( m_bTracking )
	{
		if ( m_hOldCursor == NULL )
		{
			HCURSOR Hand = ::LoadCursor(0, IDC_HAND);
			m_hOldCursor = ::SetCursor(Hand);
		}
	}
	else
	{
		if ( m_hOldCursor != NULL )
		{
			::SetCursor(m_hOldCursor);
			m_hOldCursor = NULL;
		}
	}
	return TRUE;
}
BOOL CIconBitmapButton::PreTranslateMessage(MSG* pMsg)
{
	if ( m_pToolTip != NULL )
	{
		m_pToolTip->RelayEvent(pMsg);
	}

	return CBitmapButton::PreTranslateMessage(pMsg);
}
