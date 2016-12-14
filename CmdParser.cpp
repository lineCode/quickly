#include "StdAfx.h"
#include "CmdParser.h"
#include <direct.h>
#include <shlwapi.h>
#include <PowrProf.h>
#include <Winnetwk.h>
#pragma comment(lib, "PowrProf.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Mpr.lib")
#pragma warning(disable:4996)

BOOL CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam)
{
	CCmdParser* pParser = (CCmdParser*) lParam;
	HWND hWndSelf = pParser->GetCommandWindow();

	DWORD dwExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE );

	BOOL bNoAct = dwExStyle & WS_EX_NOACTIVATE ? TRUE : FALSE;

	if ( IsWindowVisible(hWnd) && hWnd != hWndSelf && (!bNoAct) )
	{
		TCHAR szText[MAX_PATH];
		TCHAR szClass[MAX_PATH];
		GetWindowText(hWnd, szText, MAX_PATH);
		GetClassName(hWnd, szClass, MAX_PATH);

		if 
		( 
			_tcscmp( szClass, _T("Shell_TrayWnd")) == NULL ||
			_tcscmp( szClass, _T("Progman")) == NULL ||
			_tcscmp( szText, _T("시작")) == NULL ||
			_tcscmp( szClass, _T("Internet Explorer_Hidden")) == NULL ||
			_tcscmp( szClass, _T("Alternate Owner")) == NULL ||
			_tcscmp( szClass, _T("PngShadowWnd")) == NULL 
		)
		{
			// Do nothing !
		}
		else
		{
			ShowWindow(hWnd, SW_MINIMIZE);
		}
		
	}
	return TRUE;
}

DWORD WINAPI Enabler(LPVOID lpParam)
{
	CCmdParser* pParser = (CCmdParser*) lpParam;
	
	TCHAR szT[MAX_PATH];
	if ( GetModuleFileName(NULL, szT, MAX_PATH) )
	{
		TCHAR* Ptr = _tcsrchr(szT, _T('\\'));
		if ( Ptr )
		{
			*Ptr = _T('\0');
			StringCchCat(szT, MAX_PATH, _T("\\ParentEnabler.exe"));
			STARTUPINFO         sInfo;
			PROCESS_INFORMATION pInfo;
			ZeroMemory(&sInfo, sizeof(sInfo));
			sInfo.cb = sizeof(sInfo);
			ZeroMemory(&pInfo, sizeof(pInfo));

			if ( CreateProcess( szT, _T("open"), NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo ) )
			{

			}
			else
			{
				//TRACE("%d", GetLastError());
			}
		}
	}

	return 0;
}


DWORD WINAPI Reload(LPVOID lpParam)
{
	CCmdParser* pParser = (CCmdParser*) lpParam;
	pParser->ReloadFile();
	return 0;
}

DWORD WINAPI HideAllWindow(LPVOID lpParam)
{
	EnumWindows(WndEnumProc, (LPARAM) lpParam);
	return 0;
}

DWORD WINAPI LockWindow(LPVOID lpParam)
{
	::LockWorkStation();
	return 0;
}

DWORD WINAPI Hibernation(LPVOID lpParam)
{
	SetSuspendState( TRUE, FALSE, FALSE);
	return 0;
}

DWORD WINAPI Exit(LPVOID lpParam)
{
	CCmdParser* pParser = (CCmdParser*) lpParam;
	HWND hWnd = pParser->GetCommandWindow();
	PostMessage(hWnd, WM_CLOSE, NULL, NULL);
	return 0;
}

DWORD CCmdParser::ReloadFile()
{
	m_mapSharedFolder.clear();
	LoadFile();

	return 0;
}

DWORD CCmdParser::LoadFile()
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

	TCHAR* pSection = new TCHAR[32767];
	if ( pSection )
	{
		DWORD dwSize = GetPrivateProfileSection(SECTION_NAME, pSection, 32767, m_szINI);
		if ( dwSize )
		{
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
						m_mapSharedFolder.insert(make_pair(wstring(Line), wstring(++Dm)));
					}
					delete[] Line;
				}
				
			}
		}
		delete[] pSection;
	}
	
	return ERROR_SUCCESS;
}

DWORD CCmdParser::SaveFile()
{
	map<wstring, wstring>::iterator IT = m_mapSharedFolder.begin();
	for(;IT != m_mapSharedFolder.end(); IT++)
	{
		WritePrivateProfileString(SECTION_NAME, IT->first.c_str(), IT->second.c_str(), m_szINI);
	}

	return ERROR_SUCCESS;
}

CCmdParser::CCmdParser(void)
{
	m_hWnd = NULL;

	InsertCommand(_T("hide"), HideAllWindow );
	InsertCommand(_T("lock"), LockWindow );
	InsertCommand(_T("hib"), Hibernation );
	InsertCommand(_T("exit"), Exit );
	InsertCommand(_T("reload"), Reload );
	InsertCommand(_T("enabler"), Enabler );

	LoadFile();
}

CCmdParser::~CCmdParser(void)
{
	SaveFile();
}

void CCmdParser::InsertCommand(const TCHAR* pszCmd, pFunc Func)
{
	m_mapEmbedCmd.insert(make_pair(pszCmd, Func));
}

void CCmdParser::SetCommandWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}

DWORD CCmdParser::Add(LPCTSTR lpCommand)
{
	DWORD dwRet = CP_NOT_CMD;

	if ( _tcsnicmp( lpCommand, _T("add "), 4 ) == 0 )
	{
		TCHAR szCmd[MAX_PATH];
		TCHAR szValue[MAX_PATH];
		_stscanf(&lpCommand[4], _T("%s %[^\n]"), szCmd, szValue);

		m_mapSharedFolder.insert(make_pair(wstring(szCmd), wstring(szValue)));
		dwRet = CP_ADD_SF;
	}

	
	if ( dwRet != CP_NOT_CMD )
		SaveFile();

	return dwRet;
}

BOOL CCmdParser::NetworkCheck(LPCTSTR lpStr )
{
// 	WNetGetConnection()
// 	WNetAddConnection() 
// 	WNetCancelConnection()
// 	WNetDisconnectDialog() 
	//DWORD dwRet = WNetConnectionDialog( m_hWnd, RESOURCETYPE_DISK );

	BOOL bRet = TRUE;
	DWORD dwResult;
	NETRESOURCE nr; 

	memset(&nr, 0, sizeof(NETRESOURCE));  
	nr.lpRemoteName = (LPWSTR)lpStr;
	nr.dwType = RESOURCETYPE_DISK;
	nr.dwScope = RESOURCE_REMEMBERED;
	nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;

	dwResult = WNetAddConnection2(&nr, // NETRESOURCE from enumeration 
		(LPTSTR) NULL,                  // no password 
		(LPTSTR) NULL,                  // logged-in user 
	//CONNECT_CMD_SAVECRED);
	CONNECT_INTERACTIVE|CONNECT_UPDATE_PROFILE);//|CONNECT_PROMPT|CONNECT_UPDATE_PROFILE);       // update profile with connect information 

	switch(dwResult)
	{
	case ERROR_DEV_NOT_EXIST:
		_tmkdir(lpStr);
		break;
	case ERROR_SUCCESS:
		break;
	case ERROR_CANCELLED:
		bRet = FALSE;
		break;
	}

	return bRet;
}

int CCmdParser::RunByShell(LPCTSTR lpCommand)
{
	int iShellRet = 0;

	if ( _tcsnicmp(lpCommand, _T("\\\\"), 2 ) == 0 )
		if ( !NetworkCheck(lpCommand) )
			return -1;	
			
	TCHAR szParams[MAX_PATH];
	StringCchCopy(szParams, MAX_PATH, lpCommand);
	TCHAR* Arguments = _tcschr(szParams, _T(' '));
	if ( Arguments != NULL )
	{
		*Arguments = _T('\0');
		Arguments++;
	}

	

	iShellRet = (int) ShellExecute(GetDesktopWindow(), _T("explore"), szParams, Arguments, NULL, SW_SHOW);

	if ( iShellRet < 32)
	{
		iShellRet = (int) ShellExecute(m_hWnd, _T("open"), szParams, Arguments, NULL, SW_SHOW);
	}

	if ( iShellRet < 32)
	{
		iShellRet = (int) ShellExecute(m_hWnd, _T("explore"), lpCommand, NULL, NULL, SW_SHOW);
	}

	if ( iShellRet < 32)
	{
		iShellRet = (int) ShellExecute(m_hWnd, _T("open"), lpCommand, NULL, NULL, SW_SHOW);
	}

	if ( iShellRet < 32 )
	{
		TCHAR szT[MAX_PATH];
		StringCchCopy(szT, MAX_PATH, lpCommand);
		if ( szT[0] == _T('"') )
		{
			TCHAR* Ptr = _tcschr(&szT[1], _T('"'));
			if ( Ptr )
			{
				Ptr++;
				*Ptr = _T('\0');
				Ptr++;
				iShellRet = (int) ShellExecute(m_hWnd, _T("open"), szT, Ptr, NULL, SW_SHOW);
			}
		}
		else
		{
			TCHAR* Ptr = _tcschr(&szT[1], _T(' '));
			if ( Ptr )
			{
				Ptr++;
				*Ptr = _T('\0');
				Ptr++;
				iShellRet = (int) ShellExecute(m_hWnd, _T("open"), szT, Ptr, NULL, SW_SHOW);
			}
		}
	}

	/*
		없는 폴더는 만들자~!
	*/
	if (  ( PathIsNetworkPath(lpCommand) || lpCommand[1] == _T(':') )
		&& iShellRet == ERROR_FILE_NOT_FOUND )
	{
		TCHAR szExt[MAX_PATH];
		_tsplitpath(lpCommand, NULL, NULL, NULL, szExt);
		if ( _tcslen(szExt) < 1 )
		{
			int iRet = _tmkdir(lpCommand);
			if ( iRet != -1 )
			{
				iShellRet = RunByShell(lpCommand);
			}
		}
	}

	return iShellRet;
}

LPCTSTR CCmdParser::GetUserAlias(LPCTSTR lpCommand)
{
	LPCTSTR lpRet = NULL;
	int iShellRet = 0;
	map<wstring, wstring>::iterator IT = m_mapSharedFolder.find(wstring(lpCommand));
	if ( IT != m_mapSharedFolder.end() )
	{
		lpRet = IT->second.c_str();
	}

	return lpRet;
}

DWORD CCmdParser::Embed(LPCTSTR lpCommand)
{
	DWORD dwRet = CP_NOT_CMD;
	map<wstring, pFunc>::iterator IT = m_mapEmbedCmd.find(wstring(lpCommand));
	if ( IT != m_mapEmbedCmd.end() )
	{
		HANDLE hThread = CreateThread(NULL, 0, (IT->second), (LPVOID)this, NULL, NULL );
		if ( hThread != NULL )
		{
			dwRet = CP_OK;
			CloseHandle(hThread);
		}
	}
	
	return dwRet;
}

void CCmdParser::SetBeforeWindow(HWND h)
{
	m_hWndBeforeActive = h;
}

DWORD CCmdParser::Move(LPCTSTR lpCommand)
{
	if ( _tcsnicmp(lpCommand, _T("mvx"), _tcslen(_T("mvx"))) == NULL )
	{
		int px = 0;
		TCHAR dir[10];
		int iret = _stscanf(&lpCommand[3], _T("%s, %d"), dir, &px);
		
		if ( _tcsicmp(dir, _T("left")) == NULL )
		{
			RECT rc;
			::GetWindowRect(m_hWndBeforeActive, &rc);
			::MoveWindow(m_hWndBeforeActive, px, rc.top, rc.right-px, rc.bottom-rc.top, TRUE);
		}

		if ( _tcsicmp(dir, _T("right")) == NULL )
		{
			RECT rc;
			::GetWindowRect(m_hWndBeforeActive, &rc);
			int width = GetSystemMetrics(SM_CXSCREEN);
			::MoveWindow(m_hWndBeforeActive, rc.left, rc.top, width-rc.left-px, rc.bottom-rc.top, TRUE);
		}

		if ( _tcsicmp(dir, _T("top")) == NULL )
		{ 
			RECT rc;
			::GetWindowRect(m_hWndBeforeActive, &rc);
			::MoveWindow(m_hWndBeforeActive, rc.left, px, rc.right-rc.left, rc.bottom-px, TRUE);
		}

		if ( _tcsicmp(dir, _T("bottom")) == NULL )
		{
			RECT rc;
			SystemParametersInfo(SPI_GETWORKAREA,0, &rc, 0);
			
			int height = rc.bottom-rc.top;
			//int height = GetSystemMetrics(SM_CYSCREEN);
			::GetWindowRect(m_hWndBeforeActive, &rc);
			::MoveWindow(m_hWndBeforeActive, rc.left, rc.top, rc.right-rc.left, height-rc.top-px, TRUE);
		}
		return CP_OK;
	}
	else
		return CP_NOT_CMD;
}

DWORD CCmdParser::Run(LPCTSTR lpCommand)
{
	DWORD dwRet = CP_OK;
	
	dwRet = Add(lpCommand);
	if ( dwRet == CP_ADD_SF )
		return dwRet;

	dwRet = Move(lpCommand);
	if ( dwRet == CP_OK )
		return dwRet;

	dwRet = Embed(lpCommand);
	if ( dwRet == CP_OK )
		return dwRet;

	LPCTSTR lpUserAlias = GetUserAlias(lpCommand);
	
	int h = RunByShell( lpUserAlias == NULL ? lpCommand : lpUserAlias ); //(int) ShellExecute(NULL, _T("open"), lpCommand, NULL, NULL, SW_SHOW);
	
	if ( h < 32)
		return CP_NOT_CMD;
	else 
		return CP_OK;
}