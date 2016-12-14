#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CViewDlg ��ȭ �����Դϴ�.

enum { BITMAPVIEW, RICHEDITVIEW };

class CViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CViewDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
