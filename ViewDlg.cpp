// ViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Quicker.h"
#include "ViewDlg.h"


// CViewDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CViewDlg, CDialog)

int	CViewDlg::GetViewmode()
{
	return m_iCurrentMode;
}

void CViewDlg::ChangeViewmode(int Mode)
{
	HideControl();
	m_iCurrentMode = Mode;
	switch(Mode)
	{
	case BITMAPVIEW:
		RecalcView();
		ShowControl(BITMAPVIEW);
		break;
	case RICHEDITVIEW:
		ShowControl(RICHEDITVIEW);
		break;
	}
	Invalidate(TRUE);
}

CViewDlg::CViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewDlg::IDD, pParent)
{

}

CViewDlg::~CViewDlg()
{
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_VIEW, m_edtView);
	DDX_Control(pDX, IDC_RICHEDIT2, m_richView);
	DDX_Control(pDX, IDC_BMP_VIEW, m_bmpView);
	DDX_Control(pDX, IDC_HSCRBAR, m_hScroll);
	DDX_Control(pDX, IDC_VSCRBAR, m_vScroll);
}


BEGIN_MESSAGE_MAP(CViewDlg, CDialog)
	ON_WM_SIZE()
//	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CViewDlg 메시지 처리기입니다.

void CViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect orc, rc;
	int WidthVScroll = 0;

	int HeightHScroll = 0;
	GetClientRect(orc);

	if ( m_richView.GetSafeHwnd() )
	{
		m_richView.MoveWindow(orc);
	}


	if ( m_vScroll.GetSafeHwnd() )
	{
		m_vScroll.GetClientRect(rc);
		WidthVScroll = rc.Width();
	}

	if ( m_hScroll.GetSafeHwnd() )
	{
		m_hScroll.GetClientRect(rc);
		HeightHScroll = rc.Height();
	}

	if ( m_bmpView.GetSafeHwnd() )
	{
		rc.SetRect(orc.left, orc.top, orc.right-WidthVScroll, orc.bottom-HeightHScroll);
		m_bmpView.MoveWindow(rc, TRUE);
		RecalcView();
	}

	if ( m_vScroll.GetSafeHwnd() )
	{
		rc.SetRect(orc.right-WidthVScroll, orc.top, orc.right, orc.bottom-HeightHScroll);
		m_vScroll.MoveWindow(rc, TRUE);
	}

	if ( m_hScroll.GetSafeHwnd() )
	{
		rc.SetRect(orc.left, orc.bottom-HeightHScroll, orc.right-WidthVScroll, orc.bottom);
		m_hScroll.MoveWindow(rc, TRUE);
	}
}

int CViewDlg::SetData(LPVOID lpParam, int iDataSize)
{
	
	return 0; 
}

int CViewDlg::SetPlainTextW(LPCTSTR lpString)
{
	::SetWindowTextW( m_richView.GetSafeHwnd(), lpString);
	ChangeViewmode(RICHEDITVIEW);
	return 0;
}

int CViewDlg::SetPlainTextA(LPCSTR lpString)
{
	::SetWindowTextA( m_richView.GetSafeHwnd(), lpString);
	ChangeViewmode(RICHEDITVIEW);
	return 0;
}

BOOL CViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HideControl();
	CClientDC dc(this);
	m_dcMem.CreateCompatibleDC( &dc );
	ChangeViewmode(RICHEDITVIEW);
	return TRUE;
}

int CViewDlg::SetRTFText(LPCSTR lpString, UINT uCodePage)
{
	SETTEXTEX TextInfo = {0};
	TextInfo.flags = ST_SELECTION;
	TextInfo.codepage = uCodePage;
	::SetWindowTextA( m_richView.GetSafeHwnd(), "");
	::SendMessage(m_richView.GetSafeHwnd(), EM_SETTEXTEX, (WPARAM)&TextInfo, (LPARAM)lpString);
	ChangeViewmode(RICHEDITVIEW);
	return 0;
}

void CViewDlg::HideControl()
{
	m_bmpView.ShowWindow(SW_HIDE);
	m_richView.ShowWindow(SW_HIDE);
	m_edtView.ShowWindow(SW_HIDE);
	m_hScroll.ShowWindow(SW_HIDE);
	m_vScroll.ShowWindow(SW_HIDE);
}

void CViewDlg::ShowControl(int i)
{
	switch(i)
	{
	case BITMAPVIEW:
		m_bmpView.ShowWindow(SW_SHOW);
		m_hScroll.ShowWindow(SW_SHOW);
		m_vScroll.ShowWindow(SW_SHOW);
		break;
	case RICHEDITVIEW:
		m_richView.ShowWindow(SW_SHOW);
		break;
	}
}

int CViewDlg::SetBitmap(HBITMAP m_hBmpNew)
{
	memset(&m_bmInfo, 0, sizeof(m_bmInfo));
	GetObject( m_hBmpNew , sizeof(BITMAP), &m_bmInfo );
	m_hBmpOld = (HBITMAP)SelectObject(m_dcMem, m_hBmpNew );
	m_bEraseBackground = TRUE;
	
	ChangeViewmode(BITMAPVIEW);

	return 0;
}

int CViewDlg::RecalcView()
{
	if ( GetViewmode() != BITMAPVIEW ) return 0;

	sourcex = sourcey = 0;
	m_bmpView.GetClientRect( &rectStaticClient );
	rectStaticClient.NormalizeRect();
	m_size.cx = rectStaticClient.Width();    // zero based
	m_size.cy = rectStaticClient.Height();    // zero based
	m_bmpView.ClientToScreen( &rectStaticClient );
	ScreenToClient( &rectStaticClient);
	m_pt.x = rectStaticClient.left;
	m_pt.y = rectStaticClient.top;
	offsetx= m_pt.x;
	offsety=m_pt.y;
	m_vScroll.MoveWindow(offsetx+m_size.cx,offsety,18,m_size.cy);
	m_hScroll.MoveWindow(offsetx,offsety+m_size.cy,m_size.cx,18);
	horz.cbSize = sizeof(SCROLLINFO);
	horz.fMask = SIF_ALL;
	horz.nMin = 0;
	horz.nMax = m_bmInfo.bmWidth-1;// m_bmInfo.bmWidth-m_size.cx;
	horz.nPage =m_size.cx;
	horz.nPos = 0;
	horz.nTrackPos=0;

	if(m_bmInfo.bmWidth<=m_size.cx)
	{
		if((m_size.cx-m_bmInfo.bmWidth)==0)
			offsetx= m_pt.x;
		else
			offsetx= m_pt.x+((m_size.cx-m_bmInfo.bmWidth)/2);
		m_vScroll.MoveWindow(offsetx+m_bmInfo.bmWidth,offsety,18,m_size.cy);
		m_hScroll.ShowWindow(false);
	}
	else
		m_hScroll.ShowWindow(true);

	m_hScroll.SetScrollInfo(&horz);
	
	vert.cbSize = sizeof(SCROLLINFO);
	vert.fMask = SIF_ALL;
	vert.nMin = 0;
	vert.nMax =	m_bmInfo.bmHeight-1;				// m_bmInfo.bmHeight-(m_size.cy);
	vert.nPage = m_size.cy;		//*100;///m_bmInfo.bmHeight+1;
	vert.nPos = 0;
	vert.nTrackPos=0;

	if(m_bmInfo.bmHeight<=m_size.cy)
	{
		if((m_size.cy-m_bmInfo.bmHeight)==0)
			offsety= m_pt.y;
		else
			offsety= m_pt.y+((m_size.cy-m_bmInfo.bmHeight)/2);
		m_hScroll.MoveWindow(offsetx,offsety+m_bmInfo.bmHeight,m_size.cx,18);
		m_vScroll.ShowWindow(false);
	}
	else
		m_vScroll.ShowWindow(true);

	m_vScroll.SetScrollInfo(&vert);
	InvalidateRect(&rectStaticClient);
	return 0;
}


// void CViewDlg::OnPaint()
// {
// 	CPaintDC dc(this);
// 	dc.BitBlt(offsetx,offsety,m_size.cx,m_size.cy,  &m_dcMem, sourcex, sourcey,SRCCOPY);
// 	erase=false;
// 	CDialog::OnPaint();
// }


void CViewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&si);
/*
	switch (nSBCode)
	{
	case SB_TOP:
		sourcey = 0;
		break;
	case SB_BOTTOM:
		sourcey = INT_MAX;
		break;
	case SB_THUMBTRACK:
		//sourcey = nPos;
		sourcey = si.nTrackPos;
		break;
	case SB_PAGEDOWN:
	case SB_LINEDOWN:
		
		break;
	}
*/
	int nPageLines = m_size.cy;
	int nLineCount = m_bmInfo.bmHeight;

	int nNewTopLine;
	BOOL bDisableSmooth = TRUE;
	switch (nSBCode)
	{
	case SB_TOP:
		nNewTopLine = 0;
		bDisableSmooth = FALSE;
		break;
	case SB_BOTTOM:
		nNewTopLine = nLineCount - nPageLines + 1;
		bDisableSmooth = FALSE;
		break;
	case SB_LINEUP:
		nNewTopLine = sourcey - 1;
		break;
	case SB_LINEDOWN:
		nNewTopLine = sourcey + 1;
		break;
	case SB_PAGEUP:
		nNewTopLine = sourcey - si.nPage + 1;
		bDisableSmooth = FALSE;
		break;
	case SB_PAGEDOWN:
		nNewTopLine = sourcey + si.nPage - 1;
		bDisableSmooth = FALSE;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewTopLine = si.nTrackPos;
		break;
	default:
		return;
	}

	if (nNewTopLine < 0)
		nNewTopLine = 0;

	if (nNewTopLine + nPageLines >= nLineCount)
		nNewTopLine = nLineCount - nPageLines;

	//TRACE("%d,%d\n", nSBCode, nPos);

	//ScrollWindow(0,-(x-xOrig));
	sourcey = nNewTopLine;
	m_vScroll.SetScrollPos(sourcey);
	m_bEraseBackground=true;
	InvalidateRect(&rectStaticClient);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CViewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&si);
/*
	switch (nSBCode)
	{
	case SB_TOP:
		sourcex = 0;
		break;
	case SB_BOTTOM:
		sourcex = INT_MAX;
		break;
	case SB_THUMBTRACK:
		sourcex= nPos;
		break;
	}	
*/
	int nPageChars = m_size.cx;
	int nMaxLineLength = m_bmInfo.bmWidth;

	int nNewOffset;
	switch (nSBCode)
	{
	case SB_LEFT:
		nNewOffset = 0;
		break;
	case SB_BOTTOM:
		nNewOffset = nMaxLineLength - nPageChars + 1;
		break;
	case SB_LINEUP:
		nNewOffset = sourcex - 1;
		break;
	case SB_LINEDOWN:
		nNewOffset = sourcex + 1;
		break;
	case SB_PAGEUP:
		nNewOffset = sourcex - si.nPage + 1;
		break;
	case SB_PAGEDOWN:
		nNewOffset = sourcex + si.nPage - 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewOffset = si.nTrackPos;
		break;
	default:
		return;
	}

	if (nNewOffset + nPageChars >= nMaxLineLength)
		nNewOffset = nMaxLineLength - nPageChars;
	
	if (nNewOffset < 0)
		nNewOffset = 0;

	sourcex = nNewOffset;
	m_hScroll.SetScrollPos(sourcex);
	//Invalidate();
	m_bEraseBackground=true;
	InvalidateRect(&rectStaticClient);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CViewDlg::OnPaint()
{
	CPaintDC dc(this);
	dc.BitBlt(offsetx,offsety,m_size.cx,m_size.cy, &m_dcMem, sourcex, sourcey, SRCCOPY);
	m_bEraseBackground=false;
}

BOOL CViewDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bEraseBackground)
		return TRUE;
	else
		return CDialog::OnEraseBkgnd(pDC);
}