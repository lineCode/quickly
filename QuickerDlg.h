
// QuickerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ColorEdit.h"
#include "CmdParser.h"
#include "FrameShadow.h"
#include "ClipboardDlg.h"
// CQuickerDlg 대화 상자
class CQuickerDlg : public CDialog
{
// 생성입니다.
public:
	CQuickerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_QUICKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	TCHAR	m_szINI[MAX_PATH];
	HICON	m_hIcon;
	UINT	m_uiHotkey;
	HBRUSH	m_brEditBackColor;
	CFont	m_ftHistory;
	BOOL	m_bMoveHistory;
	BOOL	m_bShowMessage;
	CString m_strShowMessage;
	int		m_iHistoryCursor;
	UINT	m_iMod;
	short	m_sKey;
	//CFrameShadow* m_pFrameShadow;
	CClipboardDlg* m_pClipBoardDlg;
	vector<wstring> m_vecHistory;
	HWND	m_hWndActive;
	HANDLE m_hThreadCleaner;
	LPVOID m_lpShadowThreadContext;

	void LoadSettings();
	void SaveSettings();
	void BeginHistory(UINT Key);
	void EndHistory();
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	CColorEdit m_ctrlCmd;
	CCmdParser m_Parser;
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void SetTransparentValue(int iValue, BOOL bActivate);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	int ReSetting(void);
};
