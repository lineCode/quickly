// ColorStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "WindowColorManager.h"
#include "ColorStatic.h"
#include <strsafe.h>

// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
: m_dwColor(RGB(255,0,255))
, m_pColorCodeString(NULL)
, m_pColorDescString(NULL)
{

}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CColorStatic::SetColor(COLORREF color)
{
	m_dwColor = color;
}

void CColorStatic::SetCode(int code)
{
	m_iCode = code;
}
int CColorStatic::GetCode()
{
	return m_iCode;
}

void CColorStatic::SetColorCodeDesc(TCHAR* Code, TCHAR* Desc)
{
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	m_pColorCodeString = Code;
	m_pColorDescString = Desc;


	CRect rc;
	GetClientRect(rc);
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(this, m_pColorDescString );// , rc, 1);
	m_ToolTip.Activate(TRUE);
}

TCHAR* CColorStatic::GetColorCodeString()
{
	return m_pColorCodeString;
}

TCHAR* CColorStatic::GetColorDescString()
{
	return m_pColorDescString;
}

void CColorStatic::SetDrawItem(int f, int s)
{
	m_f = f;
	m_s = s;
}

void CColorStatic::OnPaint()
{
	CPaintDC dc(this);
	CRect rc, rcColor, rcCode, rcDesc;
	GetClientRect(&rc);
	if ( IsWindowEnabled() )
		dc.FillSolidRect(rc, RGB(255,255,255));
	else
		dc.FillSolidRect(rc, RGB(100,100,100));
	rc.DeflateRect(1,1);
	dc.FillSolidRect(rc, m_dwColor);

//	rcColor.SetRect(3, 3, 3+100, rc.Height()-3);
//	m_rcColor = rcColor;

//	rcCode.SetRect( 100+6, 3, rc.Width()-3, 32);
//	rcDesc.SetRect( 100+6, 32+3, rc.Width()-3, rc.Height()-3);
	
//	dc.FillSolidRect(rcColor, RGB(0,0,0));
//	rcColor.DeflateRect(1,1);
//	dc.FillSolidRect(rcColor, m_dwColor);
	
// 	TCHAR szT[MAX_PATH];
// 	StringCchPrintf(szT
// 		, MAX_PATH
// 		, _T("%03d %03d %03d")
// 		, GetRValue(m_dwColor)
// 		, GetGValue(m_dwColor)
// 		, GetBValue(m_dwColor));

	DWORD dwOpColor = RGB( ~GetRValue(m_dwColor), ~GetGValue(m_dwColor), ~GetBValue(m_dwColor));

	if ( (GetRValue(m_dwColor)>120 && GetRValue(m_dwColor)<136) 
		&& (GetGValue(m_dwColor)>120 && GetGValue(m_dwColor)<136) 
		&& (GetBValue(m_dwColor)>120 && GetBValue(m_dwColor)<136)  )
	{
		dwOpColor = RGB(255,255,255);
	}
		
	dc.SetTextColor( dwOpColor);
	dc.DrawText(m_pColorCodeString, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER );

//	dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
//	dc.SetBkColor( GetSysColor( COLOR_WINDOW ));
//	dc.FillSolidRect(rcCode, GetSysColor( COLOR_WINDOW ) );
//	dc.DrawText(m_pColorCodeString, &rcCode, DT_SINGLELINE | DT_LEFT | DT_VCENTER );

//	dc.SetTextColor( GetSysColor( COLOR_INFOTEXT ) );
//	dc.SetBkColor( GetSysColor( COLOR_INFOBK ));
//	dc.FillSolidRect(rcDesc, GetSysColor( COLOR_INFOBK ) );
//	dc.DrawText(m_pColorDescString, &rcDesc, DT_LEFT | DT_WORDBREAK );

}

BOOL CColorStatic::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

void CColorStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rc;
	GetClientRect(rc);

	if ( rc.PtInRect(point) )
	{
		HCURSOR h = AfxGetApp()->LoadStandardCursor(IDC_HAND);
		m_hCursor = ::SetCursor(h);
	}
	else
	{
		SetCursor(m_hCursor);
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CColorStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	CColorDialog dlg(m_dwColor, CC_FULLOPEN, this); //CC_FULLOPEN|CC_PREVENTFULLOPEN|CC_SHOWHELP|CC_ENABLETEMPLATE|CC_SOLIDCOLOR|CC_ANYCOLOR
	if ( IDOK == dlg.DoModal() )
	{
		m_dwColor = dlg.GetColor();
		Invalidate(FALSE);
//		DWORD dwNewColor = 
//		if ( SetSysColors( 1, (const INT *)&m_iCode, (const COLORREF *)&dwNewColor) )
//		{
//			ReloadColor();
//		}
	}

	//CStatic::OnLButtonDown(nFlags, point);
}

DWORD CColorStatic::GetColor()
{
	return m_dwColor;
}

void CColorStatic::ReloadColor()
{
	m_dwColor = GetSysColor( m_iCode );
	Invalidate(TRUE);
}
BOOL CColorStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
// 	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
// 
// 	return CStatic::OnEraseBkgnd(pDC);
}
