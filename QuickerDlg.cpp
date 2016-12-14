#include "stdafx.h"
#include "Quicker.h"
#include "QuickerDlg.h"
#include "SetupDlg.h"
#include "Registry.h"
#include "../zxUpdates/zxUpdatesLib/zxUpdatesLib.h"
#include "PngShadowWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CQuickerDlg::CQuickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bMoveHistory = FALSE;
	m_bShowMessage = FALSE;
//	m_pFrameShadow = NULL;
	m_pClipBoardDlg = NULL;
}

void CQuickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_ctrlCmd);
}

BEGIN_MESSAGE_MAP(CQuickerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_HOTKEY()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_TIMER()
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CQuickerDlg::SetTransparentValue(int iValue, BOOL bActivate)
{
	int I = iValue * 255 / 100;
	SetLayeredWindowAttributes( 0, I, LWA_ALPHA);
	
// 	if (  m_pFrameShadow != NULL )	
// 		m_pFrameShadow->SetTransparent(I, bActivate);
}

void CQuickerDlg::LoadSettings()
{
	TCHAR szT[MAX_PATH];
	GetModuleFileName(NULL, szT, MAX_PATH);
	TCHAR* Ptr = _tcsrchr(szT, _T('\\'));
	if ( Ptr )
	{
		*Ptr	= _T('\0');
		StringCchCat(szT, MAX_PATH, SETTING_INI );
	}
	
	StringCchCopy(m_szINI, MAX_PATH, szT);


	if ( !GetPrivateProfileStruct( SETTING_INI_SECTION, SETTING_INI_KEY, &Setting, sizeof(Setting), m_szINI) )
	{
		Setting.UseAutoStart = FALSE;

		Setting.UseColor = FALSE;
		Setting.FaceColor = DIALOG_FACE_COLOR;
		Setting.FontColor =  EDITBOX_FONT_COLOR ;
		
		Setting.UseTrnspt = FALSE;
		Setting.ForeTrnspt = 90;
		Setting.BackTrnspt = 10;

		Setting.UseHotkey = FALSE;
		Setting.ModKey =  0; // MOD_WIN | MOD_CONTROL;
		Setting.KeyCode = 0; // VK_SPACE;
	}

	ModifyStyleEx( WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED );

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	if ( !GetPrivateProfileStruct(SETTING_INI_POSITION, SETTING_INI_KEY, (LPBYTE)&wp, sizeof(wp), m_szINI) )
	{
		RECT rc;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		SetWindowPos(
			&wndNoTopMost
			, rc.right - DIALOG_WIDHT - DIALOG_SPACE
			, rc.bottom - DIALOG_HEIGHT - DIALOG_SPACE
			, DIALOG_WIDHT
			, DIALOG_HEIGHT
			, SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowPlacement(&wp);
	}

	Setting.hCurrentDialog = GetSafeHwnd();
}

void CQuickerDlg::SaveSettings()
{
	TCHAR szT[MAX_PATH];
	GetModuleFileName(NULL, szT, MAX_PATH);
	TCHAR* Ptr = _tcsrchr(szT, _T('\\'));
	if ( Ptr )
	{
		*Ptr	= _T('\0');
		StringCchCat(szT, MAX_PATH, SETTING_INI );
	}
	StringCchCopy(m_szINI, MAX_PATH,szT);

	WritePrivateProfileStruct( SETTING_INI_SECTION, SETTING_INI_KEY, &Setting, sizeof(Setting), m_szINI);
}

DWORD WINAPI CleanWindow(LPVOID lpParam)
{
	for(;;)
	{
		HWND h = FindWindow(_T("49B46336-BA4D-4905-9824-D282F05F6576"), NULL );
		if ( h != NULL )
		{
			WaitForSingleObject(GetCurrentThread(), 5000);
			::PostMessage(h, WM_CLOSE, NULL, NULL);
		}


		h = FindWindow(NULL, _T("MyPCToast"));
		if ( h != NULL )
		{
			DWORD Style = ::GetWindowLong(h, GWL_EXSTYLE);
			Style ^= WS_EX_TOPMOST;
			LONG Ret = ::SetWindowLong(h, GWL_EXSTYLE, 0);
			OutputDebugString(L"");
			ShowWindow(h, SW_HIDE);
			::PostMessage(h, WM_CLOSE, 0, 0);
			::PostMessage(h, WM_QUERYENDSESSION, 0, 0);
			::PostMessage(h, WM_QUIT, 0, 0);
		}

		WaitForSingleObject(GetCurrentThread(), 250);
	}
}

BOOL CQuickerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// zxCheckUpdates();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	LoadSettings();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CFont font;
	LOGFONT lf;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	font.GetLogFont(&lf);
	
	StringCchCopy(lf.lfFaceName, 32, _T("Georgia"));
	lf.lfHeight = -14;
	lf.lfWeight = FW_BOLD;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfQuality = CLEARTYPE_QUALITY;

	static CFont myfont;
	myfont.CreateFontIndirect(&lf);
	m_ctrlCmd.SetFont(&myfont, TRUE);

	StringCchCopy(lf.lfFaceName, 32, _T("Arial"));
	lf.lfHeight = -10;
	lf.lfWeight = FW_NORMAL;
	lf.lfQuality = DEFAULT_QUALITY;
	m_ftHistory.CreateFontIndirect(&lf);

 	m_brEditBackColor = CreateSolidBrush( Setting.FaceColor );
	m_Parser.SetCommandWindow(GetSafeHwnd());

// 	if ( m_pFrameShadow == NULL )
// 	{	
// 		
// 		m_pFrameShadow = new CFrameShadow(
// 			GetSafeHwnd()
// 			, AfxGetInstanceHandle()
// 			, rc.right - DIALOG_WIDHT - DIALOG_SPACE
// 			, rc.bottom - DIALOG_HEIGHT - DIALOG_SPACE
// 			, DIALOG_WIDHT
// 			, DIALOG_HEIGHT );
// 	}

	m_uiHotkey = GlobalAddAtom(_T("Quickly"));

	OutputDebugStringW(L"AAA");
	OutputDebugStringW(L"AAA");
	OutputDebugStringW(L"AAA");
	OutputDebugStringW(L"AAA");
	if ( Setting.UseHotkey )
	{
		if ( !RegisterHotKey(GetSafeHwnd(), m_uiHotkey, Setting.ModKey, Setting.KeyCode ) )
		{
			CString txt;
			txt.LoadString(IDS_HOTKEY_FAIL);
			AfxMessageBox(txt, MB_OK | MB_ICONERROR);
		}
		else
		{
			m_iMod = Setting.ModKey;
			m_sKey = Setting.KeyCode;
		}
	}


	if ( m_pClipBoardDlg == NULL )
	{
		m_pClipBoardDlg = new CClipboardDlg;
		if ( m_pClipBoardDlg )
		{
			m_pClipBoardDlg->Create(CClipboardDlg::IDD, this);
			//m_pClipBoardDlg->ShowWindow(SW_HIDE);
		}
	}

	m_hThreadCleaner = CreateThread(NULL, 0, CleanWindow, NULL, 0, NULL);
	
	LPPSWT_DATA pPngShadow = new PSWT_DATA;
	pPngShadow->hInstance = AfxGetInstanceHandle();
	pPngShadow->hWndParent = GetSafeHwnd();
	CreateThread(NULL, 0, PngShadowWndThread, (LPVOID)pPngShadow, 0, NULL);
	m_lpShadowThreadContext = pPngShadow;
	return TRUE;
}

void CQuickerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CQuickerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);


		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

	}
}

HCURSOR CQuickerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CQuickerDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, Setting.FaceColor);
	
	if ( m_bMoveHistory )
	{
		TCHAR szT[MAX_PATH];
		StringCchPrintf(szT, MAX_PATH, _T("%d/%d  "), m_iHistoryCursor+1, m_vecHistory.size());
		CFont* OldFont = pDC->SelectObject(&m_ftHistory);
		pDC->SetTextColor( Setting.FontColor );
		pDC->DrawText( szT, -1, rc, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM );
		pDC->SelectObject(OldFont);
		
	}

	if ( m_bShowMessage )
	{
		TCHAR szT[MAX_PATH];
		StringCchPrintf(szT, MAX_PATH, _T("    %s"), m_strShowMessage);
		CFont* OldFont = pDC->SelectObject(&m_ftHistory);
		pDC->SetTextColor( Setting.FontColor );
		pDC->DrawText( szT, -1, rc, DT_SINGLELINE | DT_LEFT | DT_BOTTOM );
		pDC->SelectObject(OldFont);
	}

	return TRUE;
}

LRESULT CQuickerDlg::OnNcHitTest(CPoint point)
{
	LRESULT lRet = CDialog::OnNcHitTest(point);
	return lRet == HTCLIENT ? HTCAPTION : lRet;
}

void CQuickerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	m_hWndActive = ::GetForegroundWindow();
	m_Parser.SetBeforeWindow(m_hWndActive);
	SetForegroundWindow();
	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}

HBRUSH CQuickerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	case CTLCOLOR_MSGBOX:
		break;
	case CTLCOLOR_EDIT:
		pDC->SetTextColor( Setting.FontColor );
		pDC->SetBkColor(  Setting.FaceColor );
		hbr = (HBRUSH) m_brEditBackColor; 
		return hbr;		
		break;
	case CTLCOLOR_LISTBOX:
		break;
	case CTLCOLOR_BTN:
		break;
	case CTLCOLOR_DLG:
		break;
	case CTLCOLOR_SCROLLBAR:
		break;
	case CTLCOLOR_STATIC:
		break;
	}
		
	return hbr;
}
void CQuickerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	//TRACE("Activate\n");

	if ( nState == WA_ACTIVE )
	{
		SetTransparentValue(Setting.ForeTrnspt, TRUE);
	}
	else if ( nState == WA_INACTIVE )
	{
		EndHistory();
		m_ctrlCmd.SetWindowText(_T(""));
		SetTransparentValue(Setting.BackTrnspt, FALSE);
	}
}

void CQuickerDlg::BeginHistory(UINT Key)
{
	if ( m_bMoveHistory )
	{
		switch(Key)
		{
		case VK_UP:
			m_iHistoryCursor--;
			break;
		case VK_DOWN:
			m_iHistoryCursor++;
			break;
		}

		if ( m_iHistoryCursor < 0 )
		{
			m_iHistoryCursor = m_vecHistory.size() - 1;
		}
		else if ( m_iHistoryCursor >= (int) m_vecHistory.size() )
		{
			m_iHistoryCursor = 0;
		}

		m_ctrlCmd.SetWindowText(m_vecHistory[m_iHistoryCursor].c_str());
	}
	else
	{
		m_iHistoryCursor = m_vecHistory.size();
		if ( m_iHistoryCursor != 0 )
		{
			switch(Key)
			{
			case VK_UP:
				m_iHistoryCursor--;
				break;
			case VK_DOWN:
				m_iHistoryCursor = 0;
				break;
			}
			m_ctrlCmd.SetWindowText(m_vecHistory[m_iHistoryCursor].c_str());
			m_bMoveHistory = TRUE;
		}
	}

	Invalidate(TRUE);
	m_ctrlCmd.SetSel(-1, 0);
}

void CQuickerDlg::EndHistory()
{
	CRect rc;
	m_bMoveHistory = FALSE;
	Invalidate(TRUE);
	m_ctrlCmd.Invalidate(FALSE);
}

BOOL CQuickerDlg::PreTranslateMessage(MSG* pMsg)
{
//	BOOL bCtrlKey = GetAsyncKeyState(VK_CONTROL)&0x8000 ? TRUE : FALSE;

	
	if(pMsg->message == WM_SYSKEYDOWN)
	{
		if(pMsg->wParam == VK_F4)
			goto RT;//return TRUE;
	}

	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_ESCAPE ||
			pMsg->wParam == VK_CANCEL )
		{
			CString txt;
			m_ctrlCmd.GetWindowText(txt);
			if ( txt.GetLength() == 0 )
				::SetForegroundWindow(m_hWndActive);
			else
				m_ctrlCmd.SetWindowText(_T(""));
			goto RT;//return TRUE;
		} 
		else if ( pMsg->wParam == VK_RETURN ) 
		{
			if ( pMsg->hwnd == m_ctrlCmd.GetSafeHwnd() )
			{
				CString text;
				m_ctrlCmd.GetWindowText(text);
			
				if ( text.CompareNoCase(_T("setup")) == 0 )
				{
					CSetupDlg dlg;
					if ( IDOK == dlg.DoModal() )
					{
						ReSetting();
					}

					goto RT;//return TRUE;
				}

				if ( text.CompareNoCase(_T("window")))

				if ( text.CompareNoCase(_T("cb")) == 0 )
				{
					if ( !m_pClipBoardDlg->IsWindowVisible() )
						m_pClipBoardDlg->ShowWindow(SW_SHOW);

					m_ctrlCmd.SetWindowText(_T(""));
					goto RT;
				}

				m_ctrlCmd.SetWindowText(_T("Processing..."));
				m_ctrlCmd.SendMessage(WM_KILLFOCUS, NULL, NULL);
				
				UINT iRet = m_Parser.Run(text);
				if ( iRet != CP_NOT_CMD )
				{
					wstring cmd(text);
					m_vecHistory.push_back(cmd);
				}

				switch ( iRet )
				{
				case  CP_ADD_SF:
					m_strShowMessage.Format(_T("OK. %d."), m_Parser.GetSFCount());
					break;
				case CP_NOT_CMD:
					m_strShowMessage.Format(_T("No Cmd (LE:%d)"), GetLastError());
					break;
				case CP_OK:
					m_strShowMessage.Format(_T(""));
					break;
				}
				m_ctrlCmd.SendMessage(WM_SETFOCUS, NULL, NULL);

				m_bShowMessage = TRUE;
				Invalidate(FALSE);
				SetTimer( 1, 1000, NULL );

				EndHistory();
			}

			m_ctrlCmd.SetWindowText(_T(""));
			goto RT;//return TRUE;
		}

// 		if ( bCtrlKey &&
// 			(
// 			pMsg->wParam == 0x4A || // 'J' LEFT  
// 			pMsg->wParam == 0x55 ||// 'U' LEFT-UP
// 			pMsg->wParam == 0x4D ||// 'M' LEFT-Down
// 			pMsg->wParam == VK_RIGHT 
// 			)
// 			)
// 		{
// 			::SetForegroundWindow(m_hWndActive);
// 			int X = pMsg->wParam == VK_LEFT ? -5 : pMsg->wParam == VK_RIGHT ? 5 : 0;
// 			int Y = pMsg->wParam == VK_UP ? -5 : pMsg->wParam == VK_DOWN ? 5 : 0;
// 			mouse_event( MOUSEEVENTF_MOVE, X, Y, 0, 0);
// 			return TRUE;
// 		}
		
		if ( pMsg->wParam == VK_UP )
		{
			BeginHistory(VK_UP);
		}
		else if ( pMsg->wParam == VK_DOWN )
		{
			BeginHistory(VK_DOWN);
		}
		else if ( pMsg->wParam == VK_BACK )
		{
			EndHistory();
		}

	}

	return CDialog::PreTranslateMessage(pMsg);

RT:
	return TRUE;
}
BOOL CQuickerDlg::OnNcActivate(BOOL bActive)
{
	//TRACE("NCActivate\n");

	SetTransparentValue( bActive ? Setting.ForeTrnspt : Setting.BackTrnspt, bActive );

	return CDialog::OnNcActivate(bActive);
}
void CQuickerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent )
	{
	case 1:
		m_bShowMessage = FALSE;
		Invalidate(TRUE);
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CQuickerDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return TRUE;
	return CDialog::OnHelpInfo(pHelpInfo);
}

void CQuickerDlg::OnClose()
{
	CDialog::OnClose();
}

void CQuickerDlg::OnMove(int x, int y)
{
	//m_pFrameShadow->Move(x,y);
	CDialog::OnMove(x, y);
}

void CQuickerDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	//m_pFrameShadow->Moving(pRect);
	CDialog::OnMoving(fwSide, pRect);
}
void CQuickerDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);

}
void CQuickerDlg::OnDestroy()
{
	if ( m_pClipBoardDlg != NULL )
	{
		::SendMessage(m_pClipBoardDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
		delete m_pClipBoardDlg;
		//m_pClipBoardDlg->EndDialog(0);
		//m_pClipBoardDlg->DestroyWindow();
		//delete m_pClipBoardDlg;
	}

	LPPSWT_DATA lp = (LPPSWT_DATA) m_lpShadowThreadContext;
	::SendMessage(lp->hWndCurrent, WM_CLOSE, 0, 0);

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	WritePrivateProfileStruct(SETTING_INI_POSITION, SETTING_INI_KEY, (LPBYTE)&wp, sizeof(wp), m_szINI);
	TerminateThread(m_hThreadCleaner, 0);

	SaveSettings();

//	delete m_pFrameShadow;
//	m_pFrameShadow = NULL;

	UnregisterHotKey(GetSafeHwnd(), m_uiHotkey);
	GlobalDeleteAtom(m_uiHotkey);
	CDialog::OnDestroy();
}
int CQuickerDlg::ReSetting(void)
{
	Invalidate(FALSE);
	
	UINT Mod;// = MOD_CONTROL | MOD_WIN;
	short Key;// = VK_SPACE;

	UnregisterHotKey(GetSafeHwnd(), m_uiHotkey);

	if ( Setting.UseHotkey )
	{
		Mod = Setting.ModKey;
		Key = Setting.KeyCode;
	
//		if ( Mod != m_iMod || Key != m_sKey )
		{
			
			if ( !RegisterHotKey( GetSafeHwnd(), m_uiHotkey, Mod, Key) )
			{
				CString txt;
				txt.LoadString(IDS_HOTKEY_FAIL);
				AfxMessageBox(txt, MB_OK | MB_ICONERROR);
			}
			else
			{
				m_iMod = Mod;
				m_sKey = Key;
			}
		}
	}

	CRegistry Reg;
	if ( Reg.Open(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), HKEY_CURRENT_USER ) )
	{
		if ( Setting.UseAutoStart )
		{
			TCHAR szT[MAX_PATH];
			GetModuleFileName(NULL, szT, MAX_PATH);
			Reg[_T("Quickly")] = szT;
		}
		else
		{
			Reg[_T("Quickly")].Delete();
		}

		Reg.Close();
	}

	if ( Setting.UseColor )
	{
		
	}
	else
	{
		Setting.FaceColor = DIALOG_FACE_COLOR;
		Setting.FontColor = EDITBOX_FONT_COLOR;
	}

	DeleteObject( m_brEditBackColor );
	m_brEditBackColor = CreateSolidBrush( Setting.FaceColor );

	Invalidate(TRUE);

	m_Parser.ReloadFile();
	return 0;
}