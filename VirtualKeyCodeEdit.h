#pragma once


// CVirtualKeyCodeEdit

class CVirtualKeyCodeEdit : public CEdit
{
	DECLARE_DYNAMIC(CVirtualKeyCodeEdit)

public:
	CVirtualKeyCodeEdit();
	virtual ~CVirtualKeyCodeEdit();

protected:
	DECLARE_MESSAGE_MAP()

	short m_sKey;

public:
//	CToolTipCtrl	m_ToolTip;	
	void SetVirualKey(short Key);
	short GetVirtualKey();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};