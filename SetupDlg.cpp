// SetupDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Quicker.h"
#include "SetupDlg.h"


// CSetupDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetupDlg, CDialog)

CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
	, m_bUseColor(FALSE)
	, m_bUseHotkey(FALSE)
	, m_bUseTrnspt(FALSE)
	, m_bUseAutoStart(FALSE)
	, m_iForeTrnspt(0)
	, m_iBackTrnspt(0)
{
	m_pFrameShadow = NULL;
}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_FORECOLOR, m_stcForecolor);
	DDX_Control(pDX, IDC_CMB_MOD_KEY, m_ctrlCmbMod);
	DDX_Control(pDX, IDC_STC_FONTCOLOR, m_stcFontcolor);
	DDX_Check(pDX, IDC_CHK_COLOR, m_bUseColor);
	DDX_Check(pDX, IDC_CHK_HOTKEY, m_bUseHotkey);
	DDX_Check(pDX, IDC_CHK_TRANSPARENT, m_bUseTrnspt);
	DDX_Check(pDX, IDC_CHK_AUTOSTART, m_bUseAutoStart);
	DDX_Control(pDX, IDC_EDT_SHORTCUT, m_ctrlShortCut);
	DDX_Text(pDX, IDC_EDT_FORETRNSPT, m_iForeTrnspt);
	DDV_MinMaxInt(pDX, m_iForeTrnspt, 50, 100);
	DDX_Text(pDX, IDC_EDT_BACKTRNSPT, m_iBackTrnspt);
	DDV_MinMaxInt(pDX, m_iBackTrnspt, 0, 50);
	DDX_Control(pDX, IDC_LST, m_list);
// 	DDX_Control(pDX, IDC_BTN_GENERNAL, m_btnGeneral);
// 	DDX_Control(pDX, IDC_BTN_COMMAND, m_btnCommand);
// 	DDX_Control(pDX, IDC_BTN_WINDOW, m_btnWindow);
// 	DDX_Control(pDX, IDC_BTN_ADVANCE, m_btnAdvance);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHK_COLOR, &CSetupDlg::OnBnClickedChkColor)
	ON_BN_CLICKED(IDC_CHK_AUTOSTART, &CSetupDlg::OnBnClickedChkAutostart)
	ON_BN_CLICKED(IDC_CHK_TRANSPARENT, &CSetupDlg::OnBnClickedChkTransparent)
	ON_BN_CLICKED(IDC_CHK_HOTKEY, &CSetupDlg::OnBnClickedChkHotkey)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LST, &CSetupDlg::OnLvnBeginlabeleditLst)
	ON_NOTIFY(NM_RCLICK, IDC_LST, &CSetupDlg::OnNMRClickLst)
	ON_COMMAND(ID_EDIT, &CSetupDlg::OnEdit)
	ON_EN_KILLFOCUS(IDC_EDT_RUNTIME, &CSetupDlg::OnEnKillfocusEdtRuntime)
	ON_COMMAND(ID_DELETE, &CSetupDlg::OnDelete)
	ON_COMMAND(ID_NEW, &CSetupDlg::OnNew)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LST, &CSetupDlg::OnLvnColumnclickLst)
	ON_BN_CLICKED(IDC_BTN_GENERNAL, &CSetupDlg::OnBnClickedBtnGenernal)
	ON_BN_CLICKED(IDC_BTN_COMMAND, &CSetupDlg::OnBnClickedBtnCommand)
END_MESSAGE_MAP()


void CSetupDlg::InitModifyKey() 
{
	for( int i=1; i<16; i++ )
	{
		BOOL bFirstInsert = FALSE;
		TCHAR szT[MAX_PATH];
		StringCchCopy(szT, MAX_PATH, _T(""));
		
		if ( i & MOD_ALT )
		{
			if ( bFirstInsert )
				StringCchCat(szT, MAX_PATH, _T("+"));
			StringCchCat(szT, MAX_PATH, _T("Alt"));
			bFirstInsert = TRUE;
		}

		if ( i & MOD_CONTROL )
		{
			if ( bFirstInsert )
				StringCchCat(szT, MAX_PATH, _T("+"));
			StringCchCat(szT, MAX_PATH, _T("Ctrl"));
			bFirstInsert = TRUE;
		}

		if ( i & MOD_SHIFT )
		{
			if ( bFirstInsert )
				StringCchCat(szT, MAX_PATH, _T("+"));
			StringCchCat(szT, MAX_PATH, _T("Shift"));
			bFirstInsert = TRUE;
		}

		if ( i & MOD_WIN )
		{
			if ( bFirstInsert )
				StringCchCat(szT, MAX_PATH, _T("+"));
			StringCchCat(szT, MAX_PATH, _T("Win"));
		}

		m_ctrlCmbMod.InsertString( m_ctrlCmbMod.GetCount(), szT);
	}
	
	m_ctrlCmbMod.SetCurSel( Setting.ModKey-1 );
	m_ctrlShortCut.SetVirualKey( Setting.KeyCode );	

	m_iMod = Setting.ModKey;
	m_sKey = Setting.KeyCode;
}

DWORD WINAPI InsertList(LPVOID lpParam)
{
	HWND hList = (HWND) lpParam;

	TCHAR szT[MAX_PATH];
	GetModuleFileName(NULL, szT, MAX_PATH);
	TCHAR* Ptr = _tcsrchr(szT, _T('\\'));
	if ( Ptr )
	{
		*Ptr	= _T('\0');
		StringCchCat(szT, MAX_PATH, SETTING_INI );
	}
	
	TCHAR* pSection = new TCHAR[32767];
	if ( pSection )
	{
		DWORD dwSize = GetPrivateProfileSection(SECTION_NAME, pSection, 32767, szT);
		if ( dwSize )
		{
			int iCount = 0;
			for(TCHAR* Ptr = pSection; *Ptr; Ptr += (_tcslen(Ptr)+1))
			{
				int BufSize = _tcslen(Ptr)+1;
				TCHAR* Line = new TCHAR[BufSize];
				if ( Line )
				{
					StringCchCopy(Line, BufSize, Ptr );
					TCHAR* Dm = _tcschr(Line, _T('='));
					if ( Dm )
					{
						*Dm = _T('\0');

						LVITEM item;
						item.mask = LVIF_TEXT;
						item.iItem = iCount;
						item.iSubItem = 0;
						item.pszText = (LPTSTR)Line;
						item.state = 0;
						item.stateMask = 0;
						item.iImage = 0;
						item.lParam = 0;

						::SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&item); 
						
						item.iSubItem = 1;
						item.pszText = (LPTSTR) ++Dm;
						::SendMessage(hList, LVM_SETITEMTEXT, iCount, (LPARAM)&item);

						iCount++;

						//m_mapSharedFolder.insert(make_pair(wstring(Line), wstring(++Dm)));
					}
					delete[] Line;
				}

			}
		}
		delete[] pSection;
	}

	return 0;
}

BOOL CSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

// 	m_btnGeneral.SetTitle(_T("General"));
// 	m_btnGeneral.SetDescription(_T("General Settings"));
// 	m_btnGeneral.SetParentBkColor(RGB(240,239,235));
// 	m_btnGeneral.SetButtonBkColor(RGB(227,94,84));
// 	m_btnGeneral.SetFontColor(RGB(113,47,42));
// 	
// 	m_btnCommand.SetTitle(_T("Command"));
// 	m_btnCommand.SetDescription(_T("General Settings"));
// 	m_btnCommand.SetParentBkColor(RGB(240,239,235));
// 	m_btnCommand.SetButtonBkColor(RGB(155,197,69));
// 	m_btnCommand.SetFontColor(RGB(113,47,42));
// 
// 	m_btnWindow.SetTitle(_T("Windows"));
// 	m_btnWindow.SetDescription(_T("General Settings"));
// 	m_btnWindow.SetParentBkColor(RGB(240,239,235));
// 	m_btnWindow.SetButtonBkColor(RGB(196,144,105));
// 	m_btnWindow.SetFontColor(RGB(113,47,42));
// 
// 	m_btnAdvance.SetTitle(_T("Advance"));
// 	m_btnAdvance.SetDescription(_T("General Settings"));
// 	m_btnAdvance.SetParentBkColor(RGB(240,239,235));
// 	m_btnAdvance.SetButtonBkColor(RGB(89,143,196));
// 	m_btnAdvance.SetFontColor(RGB(113,47,42));

	CenterWindow(CWnd::GetDesktopWindow());
	
	m_stcForecolor.SetColor( Setting.FaceColor );
	m_stcForecolor.SetColorCodeDesc( _T("Face Color"), _T("Face Color") );
	m_stcFontcolor.SetColor( Setting.FontColor );
	m_stcFontcolor.SetColorCodeDesc( _T("Font Color"), _T("Font Color") );

	CRect rc;
	GetWindowRect(rc);

	if ( m_pFrameShadow == NULL )
	{	

		m_pFrameShadow = new CFrameShadow(
			GetSafeHwnd()
			, AfxGetInstanceHandle()
			, rc.left
			, rc.top
			, rc.Width()
			, rc.Height() );
	}

//	ModifyStyleEx( WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
//	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED );

	//m_pFrameShadow->SetTransparent(255, TRUE);
	//int I = 95*255/100;
//	SetLayeredWindowAttributes( 0, 255, LWA_ALPHA);

	InitModifyKey();

	m_Brush.CreateSolidBrush(SETTING_DIALOG_COLOR);

	m_bUseAutoStart		= Setting.UseAutoStart;
	m_bUseColor			= Setting.UseColor;
	m_bUseHotkey		= Setting.UseHotkey;
	m_bUseTrnspt		= Setting.UseTrnspt;

	m_stcForecolor.EnableWindow(m_bUseColor);
	m_stcFontcolor.EnableWindow(m_bUseColor);
	
	m_iForeTrnspt = Setting.ForeTrnspt;
	m_iBackTrnspt = Setting.BackTrnspt;

	GetDlgItem(IDC_EDT_FORETRNSPT)->EnableWindow(m_bUseTrnspt);
	GetDlgItem(IDC_EDT_BACKTRNSPT)->EnableWindow(m_bUseTrnspt);
	GetDlgItem(IDC_CMB_MOD_KEY)->EnableWindow(m_bUseHotkey);
	GetDlgItem(IDC_EDT_SHORTCUT)->EnableWindow(m_bUseHotkey);

	UpdateData(FALSE);

	CFont font;
	LOGFONT lf;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	font.GetLogFont(&lf);

	StringCchCopy(lf.lfFaceName, 32, _T("Georgia"));
//	lf.lfHeight = -11;
//	lf.lfWeight = FW_BOLD;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfQuality = CLEARTYPE_QUALITY;

	
	m_ftList.CreateFontIndirect(&lf);
	m_list.SetFont(&m_ftList);
	GetDlgItem(IDC_EDT_RUNTIME)->SetFont(&m_ftList);
	m_list.InsertColumn(0, _T("Command"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, _T("Parameters"), LVCFMT_LEFT, 480);
	//m_list.ModifyStyle(0, LVS_EDITLABELS);
	m_list.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_BORDERSELECT | LVS_EX_DOUBLEBUFFER  );

	CreateThread(NULL, 0, InsertList, (LPVOID)m_list.GetSafeHwnd(), NULL, NULL);

	return TRUE;
}

BOOL CSetupDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, SETTING_DIALOG_BORDER);
	rc.DeflateRect(1,1);
	pDC->FillSolidRect(rc, SETTING_DIALOG_COLOR);
	pDC->SetBkMode(TRANSPARENT);

//	{
//		Gdiplus::Graphics graph(pDC->GetSafeHdc());
//		Font F(L"Georgia", 50, FontStyleBoldItalic, UnitPixel);
//		//PointF P( 340.0f, 20.0f);
//		RectF R(0, 0, rc.Width(), rc.Height());
//		SolidBrush B(Color(255,0,0,0));
		
//		StringFormat stringFormat;
//		stringFormat.SetLineAlignment(StringAlignmentNear);
//		stringFormat.SetAlignment(StringAlignmentFar);

//		graph.SetTextRenderingHint(TextRenderingHintAntiAlias);
//		graph.DrawString( _T("Setup"), -1, &F, R/*P*/, &stringFormat, &B);
//		graph.ReleaseHDC(pDC->GetSafeHdc());
//	}

	return TRUE;
}

void CSetupDlg::OnClose()
{
	CDialog::OnClose();
}

void CSetupDlg::OnDestroy()
{
	if ( m_pFrameShadow != NULL )
	{
		delete m_pFrameShadow;
		m_pFrameShadow = NULL;
	}

	CDialog::OnDestroy();
}

LRESULT CSetupDlg::OnNcHitTest(CPoint point)
{
	LRESULT lRet = CDialog::OnNcHitTest(point);
	return lRet == HTCLIENT ? HTCAPTION : lRet;
	return CDialog::OnNcHitTest(point);
}

void CSetupDlg::OnMove(int x, int y)
{
	m_pFrameShadow->Move(x,y);
	//CDialog::OnMove(x, y);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CSetupDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	m_pFrameShadow->Moving(pRect);
//	CDialog::OnMoving(fwSide, pRect);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

HBRUSH CSetupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	
	//pDC->SetTextColor(RGB(255,255,255));
	
	//pDC->SetBkMode(TRANSPARENT);

 	if ( nCtlColor == CTLCOLOR_STATIC )
 	{
		pDC->SetBkColor(SETTING_DIALOG_COLOR);
		return (HBRUSH) m_Brush.GetSafeHandle();
 	}
	
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
}
void CSetupDlg::OnBnClickedChkColor()
{
	UpdateData(TRUE);

	if ( m_bUseColor )
	{
		m_stcForecolor.EnableWindow(TRUE);
		m_stcForecolor.Invalidate(FALSE);
		m_stcFontcolor.EnableWindow(TRUE);
		m_stcFontcolor.Invalidate(FALSE);
	}
	else
	{
		m_stcForecolor.EnableWindow(FALSE);
		m_stcForecolor.SetColor(Setting.FaceColor);
		m_stcForecolor.Invalidate(FALSE);
		m_stcFontcolor.EnableWindow(FALSE);
		m_stcFontcolor.SetColor(Setting.FontColor);
		m_stcFontcolor.Invalidate(FALSE);
	}
}

void CSetupDlg::OnBnClickedChkAutostart()
{
	UpdateData(TRUE);
}


void CSetupDlg::OnBnClickedChkTransparent()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_EDT_FORETRNSPT)->EnableWindow(m_bUseTrnspt);
	GetDlgItem(IDC_EDT_BACKTRNSPT)->EnableWindow(m_bUseTrnspt);
}

void CSetupDlg::OnBnClickedChkHotkey()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_CMB_MOD_KEY)->EnableWindow(m_bUseHotkey);
	GetDlgItem(IDC_EDT_SHORTCUT)->EnableWindow(m_bUseHotkey);

	if ( !m_bUseHotkey )
	{
		m_ctrlCmbMod.SetCurSel( Setting.ModKey-1 );
		m_ctrlShortCut.SetVirualKey( Setting.KeyCode );	
	}
}

void CSetupDlg::OnOK()
{
	
	CWnd* pwndCtrl = GetFocus();
	// get the control ID which is presently having the focus
	int ctrl_ID = pwndCtrl->GetDlgCtrlID();

	if ( ctrl_ID == IDC_EDT_RUNTIME )
	{
		TCHAR szT[1024];
		GetDlgItemText(IDC_EDT_RUNTIME, szT, 1024);
		m_list.SetItemText(m_iEditItem, m_iEditSubItem, szT);
		GetDlgItem(IDC_EDT_RUNTIME)->ShowWindow(SW_HIDE);
		m_list.SetFocus();
	}
	else if ( ctrl_ID == 1 )
	{
		UpdateData(TRUE);

		UINT Mod = m_ctrlCmbMod.GetCurSel()+1;
		short Key = m_ctrlShortCut.GetVirtualKey();

		if ( m_iMod != Mod || m_sKey != Key )
		{
			if ( m_bUseHotkey )
			{
				ATOM Hotkey = GlobalAddAtom(_T("Quickly_Setting"));
				BOOL bFailed = FALSE;


				if ( !RegisterHotKey( GetSafeHwnd(), Hotkey, Mod, Key) )
				{
					bFailed = TRUE;
				}
				else
				{
					UnregisterHotKey(GetSafeHwnd(), Hotkey);
				}

				GlobalDeleteAtom(Hotkey);

				if ( bFailed ) 
				{
					CString txt;
					txt.LoadString(IDS_HOTKEY_FAIL);
					AfxMessageBox(txt, MB_OK | MB_ICONERROR);
					return;
				}
				else
				{
					Setting.ModKey = m_ctrlCmbMod.GetCurSel()+1;
					Setting.KeyCode = m_ctrlShortCut.GetVirtualKey();
					Setting.UseHotkey = TRUE;
				}
			}
		}

		Setting.UseAutoStart = m_bUseAutoStart;
		Setting.UseColor = m_bUseColor;
		Setting.FaceColor = m_stcForecolor.GetColor();
		Setting.FontColor = m_stcFontcolor.GetColor();
		Setting.UseTrnspt = m_bUseTrnspt;
		Setting.ForeTrnspt = m_iForeTrnspt;
		Setting.BackTrnspt = m_iBackTrnspt;

		TCHAR szT[MAX_PATH];
		GetModuleFileName(NULL, szT, MAX_PATH);
		TCHAR* Ptr = _tcsrchr(szT, _T('\\'));
		if ( Ptr )
		{
			*Ptr	= _T('\0');
			StringCchCat(szT, MAX_PATH, SETTING_INI );
		}

		WritePrivateProfileSection(SECTION_NAME, NULL, szT);

		int Count = m_list.GetItemCount();
		for (int i=0; i<Count; i++)
		{
			CString Key = m_list.GetItemText(i, 0);
			CString Value = m_list.GetItemText(i, 1);
			WritePrivateProfileString(SECTION_NAME, Key, Value, szT);
		}

		CDialog::OnOK();
	}
	
}
void CSetupDlg::OnLvnBeginlabeleditLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CSetupDlg::OnNMRClickLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast(pNMHDR);
	UNREFERENCED_PARAMETER(pNMItemActivate);

	UINT uMenuID;

	CPoint CurrentPosition;
	::GetCursorPos(&CurrentPosition);

	INT nIndex = -1;

	m_list.ScreenToClient(&CurrentPosition);
	nIndex = m_list.HitTest(CurrentPosition);

	if (-1 == nIndex)
	{
		uMenuID = IDR_MENU_NOITEM;
	}
	else
	{
		uMenuID = IDR_MENU_ITEM;
	}
	
	::GetCursorPos(&CurrentPosition);
	CMenu MenuTemp;
	CMenu *pContextMenu = NULL;
	MenuTemp.LoadMenu(uMenuID);
	pContextMenu = MenuTemp.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, CurrentPosition.x, CurrentPosition.y, this);


	m_iEditItem    =  pNMItemActivate->iItem;
	m_iEditSubItem = pNMItemActivate->iSubItem;

	*pResult = 0;
}

void CSetupDlg::OnEdit()
{
	TCHAR szT[1024];
	CRect rect, rectList;
	
	//ListView_GetSubItemRect(m_list.GetSafeHwnd(), 
	m_list.GetSubItemRect(m_iEditItem, m_iEditSubItem,LVIR_BOUNDS, rect);

	if ( m_iEditSubItem == 0 )
		rect.right = rect.left + m_list.GetColumnWidth(0);

	rect.right -= 5;

	m_list.GetWindowRect(&rectList);
	ScreenToClient(&rectList);
	rect.MoveToXY( rect.left + rectList.left , rect.top + rectList.top );

	m_list.GetItemText(m_iEditItem, m_iEditSubItem, szT, 1024);
	SetDlgItemText(IDC_EDT_RUNTIME, szT);

	rect.DeflateRect(2,2);
	rect.OffsetRect(5,2);

	GetDlgItem(IDC_EDT_RUNTIME)->MoveWindow(&rect, TRUE);
	GetDlgItem(IDC_EDT_RUNTIME)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_RUNTIME)->SetFocus();
	
//	GetDlgItem(IDC_EDT_RUNTIME)->SetWindowPos(HWND_TOP,rect.left, rect.top,);

}

BOOL CSetupDlg::PreTranslateMessage(MSG* pMsg)
{

 	if( pMsg->message == WM_KEYDOWN )
 	{
 		if( pMsg->wParam == VK_ESCAPE )
 		{
			if ( pMsg->hwnd == GetDlgItem(IDC_EDT_RUNTIME)->GetSafeHwnd() )
			{
				GetDlgItem(IDC_EDT_RUNTIME)->ShowWindow(SW_HIDE);
				return TRUE;
			}
 		}
 	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSetupDlg::OnEnKillfocusEdtRuntime()
{
	GetDlgItem(IDC_EDT_RUNTIME)->ShowWindow(SW_HIDE);	
}

void CSetupDlg::OnDelete()
{
	m_list.DeleteItem(m_iEditItem);	
}

void CSetupDlg::OnNew()
{
	int count = m_list.GetItemCount();
	int Index = m_list.InsertItem(count, _T("Command"));
	m_list.EnsureVisible(Index, FALSE);
	m_iEditItem = Index;
	m_iEditSubItem = 0;
	OnEdit();
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	BOOL bIsAcending = (BOOL)lParamSort;
	LPCTSTR s1=(LPCTSTR)((DWORD)lParam1);
	LPCTSTR s2=(LPCTSTR)((DWORD)lParam2);
	if (bIsAcending)
		return _tcsicmp(s1, s2);
	else
		return -(_tcsicmp(s1, s2));
	return 0;
}


void CSetupDlg::OnLvnColumnclickLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nSelectedColumn = pNMLV->iSubItem; 
	if (m_nSortColumn == nSelectedColumn)
		m_bAscend = !m_bAscend;
	else
		m_nSortColumn = nSelectedColumn;

	int nItems = m_list.GetItemCount();

	CString *text = new CString[nItems];

	for (int i=0; i<nItems; i++)
	{
		text[i] = m_list.GetItemText(i, nSelectedColumn);
		m_list.SetItemData(i, (DWORD)((LPCTSTR)text[i]));
	}

	LPARAM lParamSort = m_bAscend;
	m_list.SortItems(CompareFunc, lParamSort);

	delete []text;
	*pResult = 0;
}

void CSetupDlg::AllHide()
{
	UINT Items[] = {
		IDC_STC_FORECOLOR
		,IDC_CMB_MOD_KEY
		,IDC_STC_FONTCOLOR
		,IDC_CHK_COLOR
		,IDC_CHK_HOTKEY
		,IDC_CHK_TRANSPARENT
		,IDC_CHK_AUTOSTART
		,IDC_EDT_SHORTCUT
		,IDC_EDT_FORETRNSPT
		,IDC_EDT_BACKTRNSPT
		,IDC_LST
		,IDC_STATIC1
		,IDC_STATIC2 };
	int Size = sizeof(Items)/sizeof(*Items);
	for(int i=0;i<Size;i++)
	{
		GetDlgItem(Items[i])->ShowWindow(SW_HIDE);
	}
}

void CSetupDlg::OnBnClickedBtnGenernal()
{
	AllHide();
	UINT Items[] = {
		IDC_CHK_COLOR
		,IDC_CHK_HOTKEY
		,IDC_CHK_TRANSPARENT
		,IDC_CHK_AUTOSTART
		,IDC_EDT_SHORTCUT
		,IDC_EDT_FORETRNSPT
		,IDC_EDT_BACKTRNSPT
		,IDC_CMB_MOD_KEY};
	int Size = sizeof(Items)/sizeof(*Items);
	for(int i=0;i<Size;i++)
	{
		GetDlgItem(Items[i])->ShowWindow(SW_SHOW);
	}
}

void CSetupDlg::OnBnClickedBtnCommand()
{
	AllHide();
	GetDlgItem(IDC_LST)->ShowWindow(SW_SHOW);
}