#pragma once

typedef DWORD (WINAPI* pFunc)(LPVOID lpParam);

#define CP_OK			0
#define CP_NOT_CMD		1
#define CP_ADD_SF		2

class CCmdParser
{
public:
	CCmdParser(void);
	~CCmdParser(void);
	
	void SetCommandWindow(HWND hWnd);
	HWND GetCommandWindow() { return m_hWnd; }
	void InsertCommand(const TCHAR* pszCmd, pFunc Func);
	void SetBeforeWindow(HWND);
	DWORD Add(LPCTSTR);
	DWORD Embed(LPCTSTR);
	LPCTSTR GetUserAlias(LPCTSTR lpCommand);
	DWORD Run(LPCTSTR);
	int RunByShell(LPCTSTR lpCommand);
	BOOL NetworkCheck(LPCTSTR lpStr );
	DWORD LoadFile();
	DWORD ReloadFile();
	DWORD SaveFile();
	DWORD Move(LPCTSTR);

	DWORD GetSFCount() { return m_mapSharedFolder.size(); }

protected:
	
	HWND m_hWnd;
	HWND m_hWndBeforeActive;
	map<wstring, pFunc> m_mapEmbedCmd;
	map<wstring, wstring> m_mapSharedFolder;
	TCHAR m_szINI[MAX_PATH];

};