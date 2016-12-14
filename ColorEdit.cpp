#include "stdafx.h"
#include "Quicker.h"
#include "ColorEdit.h"

IMPLEMENT_DYNAMIC(CColorEdit, CEdit)

CColorEdit::CColorEdit()
{

}

CColorEdit::~CColorEdit()
{
}


BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
//	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// BOOL CColorEdit::OnEraseBkgnd(CDC* pDC)
// {
// 	CRect rc;
// 	GetClientRect(rc);
// 	pDC->FillSolidRect(rc, RGB(255,0,0));
// 	return FALSE;
// }

void CColorEdit::OnNcPaint()
{
	CRect rect;
	CDC* pDC = GetWindowDC();
	GetWindowRect( &rect);
	rect.OffsetRect( -rect.left, -rect.top);
	pDC->FillSolidRect(&rect, Setting.FaceColor );
	ReleaseDC( pDC);
}

HBRUSH CColorEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
	return (HBRUSH)GetStockObject(BLACK_BRUSH);
	return hbr;
}