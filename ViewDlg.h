#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CViewDlg 대화 상자입니다.

enum { BITMAPVIEW, RICHEDITVIEW };

class CViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_edtView;

	int SetData(LPVOID lpParam, int iDataSize);
	int SetPlainTextW(LPCTSTR lpString);
	int SetPlainTextA(LPCSTR lpString);
	int SetRTFText(LPCSTR lpString, UINT uCodePage );
	int SetBitmap(HBITMAP hBitmap);
	virtual BOOL OnInitDialog();

private:
	BOOL m_bEraseBackground;
	CRichEditCtrl m_richView;
	CStatic m_bmpView;
	CScrollBar m_hScroll;
	CScrollBar m_vScroll;
	CPoint m_pt; 
	BITMAP m_bmInfo;
	CSize m_size; 
	SCROLLINFO horz,vert;
	CRect rectStaticClient;
	int sourcex, sourcey,offsetx,offsety;
	HBITMAP m_hBmpOld;
/*	CClientDC dc(this);*/
	CDC m_dcMem;
	
	void HideControl();
	void ShowControl(int i);
	int RecalcView();
	void ChangeViewmode(int Mode);
	int	GetViewmode();
	int m_iCurrentMode;
	

public:
//	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
