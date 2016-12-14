// ClipboardDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Quicker.h"
#include "ClipboardDlg.h"
#include "SSplitter.h"
#include <mshtmcid.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

IMPLEMENT_DYNAMIC(CClipboardDlg, CDialog)

volatile long I_AM_LIVE = 0;

#define WM_CLIPBOARDDATA WM_USER+1

CClipboardDlg::CClipboardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClipboardDlg::IDD, pParent)
{
	m_bHideFormatView = FALSE;
	m_bHideDetailView = FALSE;
}

CClipboardDlg::~CClipboardDlg()
{
}

void CClipboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_SN, m_listSN);
	DDX_Control(pDX, IDC_LST_CFTYPE, m_listCFType);
}


BEGIN_MESSAGE_MAP(CClipboardDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CLIPBOARDDATA, OnClipboardData)
	ON_COMMAND(ID_CLIPITEM, &CClipboardDlg::OnClipitem)
	//ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_NOTIFY( TTN_NEEDTEXT, NULL, OnNeedText )
	ON_COMMAND(ID_CLIPFORMAT, &CClipboardDlg::OnClipformat)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_CFTYPE, &CClipboardDlg::OnLvnItemchangedLstCftype)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_SN, &CClipboardDlg::OnLvnItemchangedLstSn)
END_MESSAGE_MAP()

const TCHAR* pszClipDefine[] = 
{
	_T("CF_NULL")
	, _T("CF_TEXT")
	, _T("CF_BITMAP")
	, _T("CF_METAFILEPICT")
	, _T("CF_SYLK")
	, _T("CF_DIF")
	, _T("CF_TIFF")
	, _T("CF_OEMTEXT")
	, _T("CF_DIB")
	, _T("CF_PALETTE")
	, _T("CF_PENDATA")
	, _T("CF_RIFF")
	, _T("CF_WAVE")
	, _T("CF_UNICODETEXT")
	, _T("CF_ENHMETAFILE")
	, _T("CF_HDROP")
	, _T("CF_LOCALE")
	, _T("CF_DIBV5")
};

BOOL IsValidWindow(HWND hWnd)
{
	if ( hWnd == NULL ) return FALSE;

	RECT rc;
	GetClientRect(hWnd, &rc);
	if ( rc.right == rc.left ) return FALSE;
	if ( rc.top == rc.bottom ) return FALSE;
	if ( !IsWindowVisible(hWnd) ) return FALSE;

	return TRUE;
}

HWND GetParentOwner(HWND hWnd)
{
	return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) ? ::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
}

HWND GetTopLevelParentOwner(HWND hWnd)
{
	HWND hWndParent = hWnd;
	HWND hWndT;
	for(;;)
	{
		hWndT = GetParentOwner(hWndParent);
		if ( !IsValidWindow(hWndT) )
			break;
		hWndParent = hWndT;
	}
	return hWndParent;
}

HWND GetTopForegroundWnd()
{
	return GetTopLevelParentOwner( GetForegroundWindow() );
}

int GetClipboardSize(HANDLE hClipboard)
{
	int iSize = 0;

	__try
	{
		iSize = GlobalSize( hClipboard );
	}
	__except(1)
	{
		iSize = 0;
	}

	return iSize;
}


DWORD WINAPI CLIPPOLL(LPVOID lpParam)
{
	int		iCount   = 0;	// Count of Clip Type
	unsigned long ulClipDataSize	 = 0;
	HWND	hOwner   = 0;	// Clip Owner Window
	HWND	hUserTop = 0;	// Clip Owner's Top Level Window
	DWORD	dwCSN	 = 0;	// Current CSN
	DWORD	dwOldCSN = 0;	// Old CSN
	DWORD	dwPid, dwTid;
	TCHAR	szTitle[MAX_PATH];
	TCHAR	szClass[MAX_PATH];
	TCHAR	szProcess[MAX_PATH];
	HWND	hWndMonitor = (HWND) lpParam;


	while( I_AM_LIVE )
	{
		dwCSN = GetClipboardSequenceNumber();

		if ( dwCSN != dwOldCSN )
		{
			LPCLIPBOARD_DATA pNewItem = new CLIPBOARD_DATA;
			if ( pNewItem == NULL )
			{
				continue;
			}
			else
				pNewItem->hWndOwner = NULL;

			StringCchCopy(szTitle, MAX_PATH, _T(""));
			StringCchCopy(szClass, MAX_PATH, _T(""));
			StringCchCopy(szProcess, MAX_PATH, _T(""));

			hOwner = GetClipboardOwner();
			iCount = CountClipboardFormats();

			if ( hOwner != NULL )
			{
				hUserTop = GetTopLevelParentOwner(hOwner);
				GetWindowText(hUserTop, szTitle, MAX_PATH);
				GetClassName(hUserTop, szClass, MAX_PATH);
				dwTid = GetWindowThreadProcessId(hOwner, &dwPid);
				if ( dwPid != 0 )
				{
					HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, dwPid);
					if ( hProc )
					{
						TCHAR szProcName[MAX_PATH];
						if ( GetProcessImageFileNameW( hProc, szProcName, MAX_PATH) )
						{
							TCHAR* Ptr = _tcsrchr(szProcName, _T('\\'));
							if ( Ptr )
							{
								StringCchCopy(szProcess, MAX_PATH, ++Ptr);
							}
						}
						CloseHandle(hProc);
					}
				}

				pNewItem->hWndOwner	= hOwner;
				pNewItem->Process	= szProcess;
				pNewItem->Title		= szTitle;
				pNewItem->Class		= szClass;
			}

			if ( OpenClipboard( hWndMonitor ) )
			{
				UINT id = 0;
				do
				{
					id = EnumClipboardFormats(id);

					if ( id != 0 )
					{
						LPCTSTR Name = NULL;
						ELEMENT Element;
						TCHAR szName[MAX_PATH] = _T("");
						
						if ( id > 17 )
						{
							GetClipboardFormatName(id, szName, MAX_PATH);
							Name = szName;
						} 
						else
						{
							Name = pszClipDefine[id];
						}

						LPVOID pTempPtr = NULL;
						HANDLE hClipboardData = GetClipboardData(id);
						if ( hClipboardData == NULL )
						{
							continue;
						}
						
						/*
							CF_BITMAP, CF_ENHMETAFILE, and CF_HDROP
							HBITMAP, HENHMETAFILE, or HDROP
						*/
						
						switch (id)
						{
						case CF_BITMAP:
						case CF_ENHMETAFILE:
						case CF_HDROP:
							{
								pTempPtr = new HANDLE;
								if ( pTempPtr != NULL )
								{
									ulClipDataSize = sizeof(HANDLE);
									memcpy(pTempPtr, &hClipboardData, ulClipDataSize );
								}
							}
							break;
						default:
							{
								ulClipDataSize = GetClipboardSize( hClipboardData );
								if ( ulClipDataSize != 0 )
								{
									pTempPtr = malloc(ulClipDataSize);
									if ( pTempPtr != NULL )
									{
										LPVOID lpSrc = GlobalLock(hClipboardData);
										if ( lpSrc != NULL )
										{
											memcpy( pTempPtr, lpSrc, ulClipDataSize );
										}
										GlobalUnlock(hClipboardData);
									}
								}
							}
							break;
						}

						Element.uID			= id;
						Element.dwSize		= ulClipDataSize;
						Element.FormatName	= Name;
						Element.pData		= pTempPtr;

						pNewItem->Elements.push_back(Element);
					}

				} while ( id != 0 );

				CloseClipboard();			
			}

			if ( pNewItem->Elements.size() == 0 )
			{
				delete pNewItem;
			}
			else
			{
				pNewItem->dwSN		= dwCSN;
				pNewItem->iCFCount	= iCount;
				SendMessage( hWndMonitor, WM_CLIPBOARDDATA, (WPARAM) pNewItem, NULL );
				dwOldCSN = dwCSN;
			}
		}

		WaitForSingleObject(GetCurrentThread(), 250);
	}

	return 0;
}

bool ElementsSort( ELEMENT e1, ELEMENT e2 )
{
	return e1.uID < e2.uID ;
}

LRESULT CClipboardDlg::OnClipboardData(WPARAM wParam, LPARAM lParam)
{
	LPCLIPBOARD_DATA pData = (LPCLIPBOARD_DATA) wParam;

	if ( pData != NULL )
	{
		std::sort( pData->Elements.begin(), pData->Elements.end(), ElementsSort);

		/* 고유번호 | 포멧개수 | 프로세스 | 윈도우 | 윈도우핸들 */

		TCHAR szString[MAX_PATH];
		StringCchPrintf(szString, MAX_PATH, _T("%d"), pData->dwSN);
		int Idx = m_listSN.InsertItem( 0, szString );
		m_listSN.SetItemData( Idx, (DWORD_PTR) (LPVOID) pData );

		StringCchPrintf(szString, MAX_PATH, _T("%d/%d"), pData->Elements.size(), pData->iCFCount);
		m_listSN.SetItemText( Idx, 1, szString);
		m_listSN.SetItemText( Idx, 2, pData->Process.c_str());
		m_listSN.SetItemText( Idx, 3, pData->Title.size() == 0 ? pData->Class.c_str() : pData->Title.c_str());
		StringCchPrintf(szString, MAX_PATH, _T("0x%08x"), pData->hWndOwner );
		m_listSN.SetItemText( Idx, 4, szString);
	}

	return 0;
}


BOOL CClipboardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDR_ACCELERATOR1));


	InterlockedExchange(&I_AM_LIVE, TRUE);

	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(5,5);
		
	m_LeftDlgSplitterPane.Create(
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|SS_HORIZ
		, this
		, &m_listSN
		, &m_listCFType
		, IDC_HORIZ_DLG
		, rc);
	
	m_pDlgView = new CViewDlg;
	m_pDlgView->Create(CViewDlg::IDD, this);
	m_pDlgView->ShowWindow(SW_SHOW);

	m_MainDlgSplitterPane.Create(
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|SS_VERT
		, this
		, &m_LeftDlgSplitterPane
		, m_pDlgView
		, IDC_VERT_DLG
		, rc);

	m_Brush.CreateSolidBrush( RGB(255,0,0));

	m_listSN.InsertColumn(0, _T("SN"), LVCFMT_LEFT, 50);
	m_listSN.InsertColumn(1, _T("Types"), LVCFMT_CENTER, 50);
	m_listSN.InsertColumn(2, _T("Process"), LVCFMT_LEFT, 100);
	m_listSN.InsertColumn(3, _T("Window"), LVCFMT_LEFT, 150);
	m_listSN.InsertColumn(4, _T("Handle"), LVCFMT_RIGHT, 85);
	m_listSN.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);

	m_listCFType.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 55);
	m_listCFType.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 180);
	m_listCFType.InsertColumn(2, _T("Size"), LVCFMT_RIGHT, 75);
	m_listCFType.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);

	/*
		TOOLBAR 
	*/
	m_ImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK,  9, 9);
	CBitmap Bitmap, bitmap2;
	Bitmap.LoadBitmap(IDB_BMP_CBITEM);
	bitmap2.LoadBitmap(IDB_BMP_CBFORMAT);

#ifdef USE_TOOLBARCTRL
	
	m_ImageList.Add(&Bitmap, RGB(255,0,255)); 
	m_ImageList.Add(&bitmap2, RGB(255,0,255)); 

	

	TBBUTTON tb;
	m_ToolBar.Create(WS_CHILD|WS_VISIBLE | TBSTYLE_TOOLTIPS //|/* WS_BORDER| TBSTYLE_FLAT |*/ // | 
		//CBRS_SIZE_DYNAMIC | CBRS_GRIPPER  | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY
		, CRect(0,0,0,0), this, 0);
	m_ToolBar.SetImageList(&m_ImageList); 
//	m_ToolBar.AddStrings(_T("TEST1\0TEST2\0"));
	
	tb.iBitmap = 0;
	tb.iString = 0;
	tb.fsState = TBSTATE_ENABLED;
	tb.fsStyle = TBSTYLE_CHECK;//TBSTYLE_BUTTON;
	tb.idCommand = ID_CLIPITEM;
	m_ToolBar.AddButtons(1, &tb);

	tb.iBitmap = 1;
	tb.iString = 1;
	tb.idCommand = ID_CLIPFORMAT;
	m_ToolBar.AddButtons(1, &tb);

// 	{
// 		CRect rect;
// 		m_ToolBar.GetItemRect(0, &rect);
// 		m_ToolBar.SetSizes(rect.Size(), CSize(16, 15));
// 	}
#else
	{
		if (!m_ToolBar.Create( this ) || !m_ToolBar.LoadToolBar(IDR_TOOLBAR) )
		{
			TRACE0("Failed to create toolbar\n");
			return -1;
		}

		m_ToolBar.SetBarStyle(m_ToolBar.GetBarStyle() | CBRS_GRIPPER | CBRS_BORDER_3D  );

		CRect rcClientStart;
		CRect rcClientNow;

		GetClientRect(rcClientStart);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

		CPoint ptOffset(rcClientNow.left - rcClientStart.left, rcClientNow.top - rcClientStart.top);

		CRect  rcChild;
		CWnd* pwndChild = GetWindow(GW_CHILD);
		while (pwndChild)
		{
			pwndChild->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			rcChild.OffsetRect(ptOffset);
			pwndChild->MoveWindow(rcChild, FALSE);
			pwndChild = pwndChild->GetNextWindow();
		}

		CRect rcWindow;
		GetWindowRect(rcWindow);
		rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
		rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
		MoveWindow(rcWindow, FALSE);

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}

#endif
		
// 		TBBUTTON tb1;
// 		memset(&tb1, 0, sizeof(tb1));
// 		tb1.fsStyle = TBSTYLE_SEP;
// 		m_ToolBar.AddButtons(1, &tb1);
// 		tb.iBitmap = 1;
// 		tb.idCommand = 0xffff;
// 		m_ToolBar.AddButtons(1, &tb);
// 		tb.iBitmap = 2;
// 		tb.idCommand = ID_DRAWING;
// 		m_ToolBar.AddButtons(1, &tb);
// 
// 		TBBUTTON tb1;
// 		tb1.fsStyle = TBSTYLE_SEP;
// 		m_ToolBar.AddButtons(1, &tb1);
// 
// 		tb.iBitmap = 3;
// 		tb.idCommand = ID_STRIKEOUT;
// 		tb.iString = NULL;
// 		m_ToolBar.AddButtons(1, &tb);

		//
		// We now Create the Statusbar as show below
		//

		m_StatusBar.Create(WS_CHILD|WS_VISIBLE|SBT_OWNERDRAW, CRect(0,0,0,0), this, 0);

		// You can set the no. of parts of the StatusBar (4 in this case) 
		// and the width of each part not -1 means remaining width

		int strPartDim[4]= {180, 260, 340, -1};
		m_StatusBar.SetParts(4, strPartDim);

		m_StatusBar.SetText(_T("Dialog / StatusBar / Toolbar"),0,0);
		m_StatusBar.SetText(_T("Example"), 1, 0);

//		CString string;
//		string.LoadString(IDS_MYCOMPUTER);
		m_StatusBar.SetText(_T("TEST"), 3 ,SBT_NOBORDERS );
		m_StatusBar.SetIcon( 3, SetIcon(AfxGetApp()->LoadIcon((UINT)0), FALSE));
	

//	m_rebar.Create(this);
//	REBARBANDINFO info;

	//m_rebar.InsertBand(0, );
//	m_rebar.AddBar(&m_ToolBar, RGB(255,0,0), RGB(0,255,0),"TB 1", RBBS_GRIPPERALWAYS| RBBS_CHILDEDGE);
	//m_rebar.AddBar(&m_SecondToolBar, RGB(255,0,0), RGB(0,255,0),"TB 2", RBBS_GRIPPERALWAYS| RBBS_CHILDEDGE);
//	- See more at: http://cppandmfc.blogspot.kr/2013/05/mfc-displaying-toolbars-on-rebar-control.html#sthash.mzMJaxkL.dpuf

	m_hPollThread = CreateThread(NULL, 0, CLIPPOLL, (LPVOID) GetSafeHwnd(), 0, NULL );

	return TRUE;
}
void CClipboardDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);
	int iHeightStatusBar = 0;
	int iHeightToolbar = 0;

	if ( m_ToolBar.GetSafeHwnd() )
	{
		CRect src;
		CRect wrc;

		m_ToolBar.GetClientRect(src);
		m_ToolBar.GetWindowRect(wrc);

		iHeightToolbar = src.Height(); // wrc.Height();//
		src = rc;
		src.top = rc.top;
		src.bottom = rc.top + iHeightToolbar;
		src.left = rc.left;
		src.right = rc.right;
		
		//::SetWindowPos( m_ToolBar.GetSafeHwnd(), GetSafeHwnd(), 0, 0, wrc.Width(), wrc.Height(), 0);
		m_ToolBar.MoveWindow(src, FALSE);

		//RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}

	if ( m_StatusBar.GetSafeHwnd() )
	{
		CRect src;
		m_StatusBar.GetClientRect(src);
		iHeightStatusBar = src.Height();
		src = rc;
		src.top = rc.bottom - iHeightStatusBar;
		src.bottom = rc.bottom;
		src.left = rc.left;
		src.right = rc.right;
		m_StatusBar.MoveWindow(src);
	}

	if ( m_MainDlgSplitterPane.GetSafeHwnd() )
	{
		CRect src;
		src = rc;
		src.DeflateRect(5,2);
		src.top += iHeightToolbar;
		src.bottom -= iHeightStatusBar;		
		m_MainDlgSplitterPane.MoveWindow(src);
	
// 		m_tab.GetWindowRect(src);
// 		ScreenToClient(src);
// 		int X = src.left;
// 		int Y = src.top;
// 		m_tab.GetClientRect(src);
// 		m_tab.AdjustRect(FALSE, src);
// 		int W = src.Width();
// 		int H = src.Height();
// 		X += src.left;
// 		Y += src.top;
// 		src.SetRect(X, Y, X+W, Y+H);
// 		m_edtView.MoveWindow(src);
	}
}

BOOL CClipboardDlg::OnEraseBkgnd(CDC* pDC)
{
//	CRect r;
//	GetClientRect(r);
//	pDC->FillSolidRect(r, RGB(137,140,149));
//	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}
HBRUSH CClipboardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//  	if ( nCtlColor == CTLCOLOR_STATIC )
//  		return (HBRUSH) m_Brush.GetSafeHandle();

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CClipboardDlg::OnDestroy()
{
	InterlockedExchange(&I_AM_LIVE, FALSE);
	
	if ( WaitForSingleObject(m_hPollThread, 5000) != WAIT_OBJECT_0 )
		TerminateThread( m_hPollThread, 0 );

	int iCount = m_listSN.GetItemCount();
	
	for(int i=0;i<iCount; i++)
	{
		DWORD_PTR Ptr = m_listSN.GetItemData(i);
		LPCLIPBOARD_DATA pData = (LPCLIPBOARD_DATA) Ptr;
		int eCount = pData->Elements.size();
		for(int j=0; j<eCount; j++)
		{
			LPVOID mp = pData->Elements[j].pData;
			if ( mp != NULL )
			{
				free(mp);
				mp = NULL;
			}
		}
		delete pData;
	}

	m_pDlgView->DestroyWindow();
	delete m_pDlgView;
	

	CDialog::OnDestroy();
}

void CClipboardDlg::OnClipitem()
{
	if ( m_bHideFormatView )
	{
		m_LeftDlgSplitterPane.ShowBottomPane();
		m_bHideFormatView = FALSE;	
#ifdef USE_TOOLBAR
		m_ToolBar.CheckButton(ID_CLIPITEM, FALSE);
#endif

	}
	else
	{
		m_LeftDlgSplitterPane.HideBottomPane();
		m_bHideFormatView = TRUE;
#ifdef USE_TOOLBAR
		m_ToolBar.CheckButton(ID_CLIPITEM, TRUE);
#endif
	}
}

BOOL CClipboardDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( m_hAccel != NULL )
	{
		if ( TranslateAccelerator( m_hWnd, m_hAccel, pMsg ) )
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CClipboardDlg::OnNeedText(NMHDR* pnmh, LRESULT* pResult)
{
	TOOLTIPTEXT* pToolTip = (TOOLTIPTEXT*)pnmh;

	switch( pToolTip->hdr.idFrom )
	{
	case ID_CLIPITEM:
		StringCchCopy( pToolTip->szText, 80, _T("포멧뷰 보기/숨기기 (F5)") );
		break;
	case ID_CLIPFORMAT:
		StringCchCopy( pToolTip->szText, 80, _T("포멧뷰어 보기/숨기기 (F6)"));
		break;
	}
}
void CClipboardDlg::OnClipformat()
{	
	if ( m_bHideDetailView )
	{
		m_MainDlgSplitterPane.ShowRightPane();
		m_bHideDetailView = FALSE;
		SetForegroundWindow();
#ifdef USE_TOOLBAR
		m_ToolBar.CheckButton(ID_CLIPFORMAT, FALSE);
#endif
	}
	else
	{
		m_MainDlgSplitterPane.HideRightPane();
		m_bHideDetailView = TRUE;
#ifdef USE_TOOLBAR
		m_ToolBar.CheckButton(ID_CLIPFORMAT, TRUE);
#endif
	}

}
void CClipboardDlg::OnLvnItemchangedLstCftype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ( pNMLV->uNewState & LVIS_SELECTED )
	{
		LPELEMENT pElement = (LPELEMENT) m_listCFType.GetItemData( pNMLV->iItem );

		if (  pElement->uID <= 17 )
		{
			switch(pElement->uID)
			{
			case CF_TEXT:
				m_pDlgView->SetPlainTextA( (LPCSTR) pElement->pData );
				break;
			case CF_BITMAP:
				m_pDlgView->SetBitmap( *reinterpret_cast<HBITMAP*>(pElement->pData) );
				break;
			case CF_METAFILEPICT:
				break;
			case CF_SYLK:
				break;
			case CF_DIF:
				break;
			case CF_TIFF:
				break;
			case CF_OEMTEXT:
				m_pDlgView->SetPlainTextA( (LPCSTR) pElement->pData );
				break;
			case CF_DIB:
			case CF_PALETTE:
			case CF_PENDATA:
			case CF_RIFF:
			case CF_WAVE:
				break;
			case CF_UNICODETEXT:
				m_pDlgView->SetPlainTextW( (LPCTSTR) pElement->pData );
				break;
			case CF_ENHMETAFILE:
			case CF_HDROP:
			case CF_LOCALE:
			case CF_DIBV5:
				break;
			} // end of switch
		}
		else
		{
			// custom clipboard
			
			BOOL bUTF8 = std::wstring::npos	!= pElement->FormatName.find(_T("UTF8")) ? TRUE : FALSE;

			if ( std::wstring::npos	!= pElement->FormatName.find(_T("RTF")))
			{
				m_pDlgView->SetRTFText( (LPCSTR) pElement->pData, bUTF8 ? CP_UTF8 : CP_ACP );
			}

			if ( std::wstring::npos	!= pElement->FormatName.find(_T("Rich Text Format")))
			{
				m_pDlgView->SetRTFText( (LPCSTR) pElement->pData, bUTF8 ? CP_UTF8 : CP_ACP );
			}

			if ( std::wstring::npos	!= pElement->FormatName.find(_T("HTML Format")))
			{
				int size = pElement->dwSize*2;
				wchar_t* ptemp = new wchar_t[size];
				if ( ptemp )
				{
					MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) pElement->pData, -1, ptemp, size );
					m_pDlgView->SetPlainTextW( ptemp );
					delete ptemp;
				}
				else
				{
					m_pDlgView->SetPlainTextA( "Not enough memory.");
				}
			}

			if ( std::wstring::npos	!= pElement->FormatName.find(_T("UniformResourceLocatorW")))
			{
				m_pDlgView->SetPlainTextW( (LPCTSTR) pElement->pData );
			}

			if ( std::wstring::npos	!= pElement->FormatName.find(_T("msSourceUrl")))
			{
				m_pDlgView->SetPlainTextW( (LPCTSTR) pElement->pData );
			}
		}
	}
	
	*pResult = 0;
}

void CClipboardDlg::OnLvnItemchangedLstSn(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ( pNMLV->uNewState & LVIS_SELECTED )
	{
		TCHAR szT[MAX_PATH];
		DWORD_PTR Ptr = m_listSN.GetItemData( pNMLV->iItem );
		LPCLIPBOARD_DATA pCD = (LPCLIPBOARD_DATA) Ptr;
		int iCount = pCD->Elements.size();

		m_listCFType.DeleteAllItems();

		for(int i=0; i<iCount; i++)
		{
			StringCchPrintf(szT, MAX_PATH, _T("%d"), pCD->Elements[i].uID);
			m_listCFType.InsertItem(i, szT);
			StringCchPrintf(szT, MAX_PATH, _T("%s"), pCD->Elements[i].FormatName.c_str());
			m_listCFType.SetItemText(i, 1, szT);
			GetFormedStringSize(pCD->Elements[i].dwSize, szT, MAX_PATH, FALSE);
			//StringCchPrintf(szT, MAX_PATH, _T("%d Bytes"), pCD->Elements[i].dwSize);
			m_listCFType.SetItemText(i, 2, szT);
			m_listCFType.SetItemData(i, (DWORD_PTR) &pCD->Elements[i] );
		}
	}

	*pResult = 0;
}