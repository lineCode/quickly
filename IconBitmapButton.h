#pragma once


// CIconBitmapButton

class CIconBitmapButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CIconBitmapButton)

public:
	CIconBitmapButton();
	virtual ~CIconBitmapButton();

	void SetTitle(CString Text);
	void SetDescription(CString Text);
	void SetParentBkColor(DWORD dwColor);
	void SetButtonBkColor(DWORD dwColor);
	void SetFontColor(DWORD dwColor);

protected:
	DECLARE_MESSAGE_MAP()

	CFont	m_fTitle;
	CFont	m_fDescription;
	BOOL	m_bTracking;
	HCURSOR m_hOldCursor;
	CString m_sTitle;
	CString m_sDescription;
	DWORD	m_crParent;
	DWORD	m_crButton;
	DWORD	m_ftButton;

	CToolTipCtrl* m_pToolTip;

	DWORD ModClr(int R, int G, int B, int BaseColor);
	DWORD GetFocusHColor();

public:
	
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};