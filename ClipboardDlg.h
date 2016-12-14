#pragma once
#include "afxcmn.h"
#include "SSplitter.h"
#include "ToolbarCompCtrl.h"
#include "afxwin.h"
#include "ViewDlg.h"

#define USE_TOOLBARCTRL

typedef struct _ELEMENT
{
	UINT uID;
	std::wstring FormatName;
	DWORD dwSize;
	LPVOID pData;
} ELEMENT, *LPELEMENT;

typedef struct _CLIPBOARD_DATA
{
	DWORD dwSN;
	HWND hWndOwner;
	int iCFCount;
	std::wstring Title;
	std::wstring Class;
	std::wstring Process;
	std::vector<ELEMENT> Elements;
} CLIPBOARD_DATA, *LPCLIPBOARD_DATA;

class CClipboardDlg : public CDialog
{
	DECLARE_DYNAMIC(CClipboardDlg)

public:
	CClipboardDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClipboardDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl		m_listSN;
	CListCtrl		m_listCFType;
	CSSplitter		m_MainDlgSplitterPane;
	CSSplitter		m_LeftDlgSplitterPane;
	CReBarCtrl		m_rebar;
	CBrush			m_Brush;
	CImageList		m_ImageList;
	HANDLE			m_hPollThread;
	CViewDlg*		m_pDlgView;
#ifdef USE_TOOLBARCTRL
	CToolBarCtrl	m_ToolBar;
#else
	CToolBar	m_ToolBar;
#endif
	CStatusBarCtrl	m_StatusBar;
	HACCEL			m_hAccel;
	BOOL			m_bHideFormatView;
	BOOL			m_bHideDetailView;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnClipboardData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClipitem();
	afx_msg void OnNeedText(NMHDR* pnmh, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClipformat();
	afx_msg void OnLvnItemchangedLstCftype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedLstSn(NMHDR *pNMHDR, LRESULT *pResult);
};
