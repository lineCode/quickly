#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	COLORREF GetColor();
	void SetColor(COLORREF color);
	void SetCode(int code);
	int GetCode();
	TCHAR* GetColorCodeString();
	TCHAR* GetColorDescString();
	void SetColorCodeDesc(TCHAR*, TCHAR*);
	void SetDrawItem(int, int);
	void ReloadColor();

protected:
	CToolTipCtrl	m_ToolTip;	
	DWORD m_dwColor;
	int m_iCode;
	TCHAR* m_pColorCodeString;
	TCHAR* m_pColorDescString;
	int m_f;
	int m_s;
	CRect m_rcColor;
	HCURSOR m_hCursor;
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
protected:
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
//	virtual void PreSubclassWindow();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};