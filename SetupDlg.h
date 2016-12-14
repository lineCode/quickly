#pragma once
#include "afxwin.h"
#include "ColorStatic.h"
#include "FrameShadow.h"
#include "VirtualKeyCodeEdit.h"
#include "afxcmn.h"
#include "IconBitmapButton.h"

class CSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetupDlg)

public:
	CSetupDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetupDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void InitModifyKey();
	CFrameShadow* m_pFrameShadow;
	virtual BOOL OnInitDialog();
	
	CColorStatic m_stcForecolor;
	CColorStatic m_stcFontcolor;
	UINT m_iMod;
	short m_sKey;
	int m_iEditItem;
	int m_iEditSubItem;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	CComboBox m_ctrlCmbMod;
	CBrush m_Brush;
	CFont m_ftList;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	BOOL m_bUseColor;
	afx_msg void OnBnClickedChkColor();
	BOOL m_bUseHotkey;
	BOOL m_bUseTrnspt;
	BOOL m_bUseAutoStart;
	CVirtualKeyCodeEdit m_ctrlShortCut;
protected:
	virtual void OnOK();
public:
	int m_iForeTrnspt;
	int m_iBackTrnspt;
	int m_nSortColumn;
	BOOL m_bAscend;
	afx_msg void OnBnClickedChkAutostart();
	afx_msg void OnBnClickedChkTransparent();
	afx_msg void OnBnClickedChkHotkey();
	CListCtrl m_list;
	afx_msg void OnLvnBeginlabeleditLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEdit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocusEdtRuntime();
	afx_msg void OnDelete();
	afx_msg void OnNew();
	afx_msg void OnLvnColumnclickLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnGenernal();
// 	CIconBitmapButton m_btnGeneral;
// 	CIconBitmapButton m_btnCommand;
// 	CIconBitmapButton m_btnWindow;
// 	CIconBitmapButton m_btnAdvance;
	afx_msg void OnBnClickedBtnCommand();
	void AllHide();
};