
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.
#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����
#include <strsafe.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


using namespace std;
using namespace Gdiplus;


BOOL GetFormedStringSize64(unsigned __int64 i64Bytes, LPTSTR pszFormed, int iBuffSize, BOOL bOnlyKB);
BOOL GetFormedStringSize(unsigned int iBytes, LPTSTR pszFormed, int iBuffSize, BOOL bOnlyKB );

#define SETTING_INI_SECTION			_T("Settings")
#define SETTING_INI_POSITION		_T("Position")
#define SETTING_INI_KEY				_T("Hash")
#define SECTION_NAME				_T("MAP")

#define EDITBOX_INNER_COLOR			RGB(227, 22, 0) //RGB(0,0,0)// RGB(255,255,255)
#define EDITBOX_FONT_COLOR			RGB(255, 255,  0)
#define EDITBOX_BORDER_COLOR		RGB(227, 22, 0) //RGB(0,0,0)//RGB(255,255,255)
#define DIALOG_FACE_COLOR			RGB(227, 22, 0) //RGB(0,0,0)//RGB(255,255,255)

#define SETTING_DIALOG_BORDER		RGB(207,207,207)//82,132,188) //RGB(72,61,139) //RGB(222,255,0) //RGB(227, 22, 0) //RGB(170,204,255)
#define SETTING_DIALOG_COLOR		RGB(240,239,235)//107,173,246)
#define SETTING_INI					_T("\\Quickly.ini")

#define DIALOG_WIDHT 140
#define DIALOG_HEIGHT 40
#define DIALOG_SPACE  25
#ifdef _UNICODE

typedef struct _SETTING
{
	HWND	hCurrentDialog;
	BOOL	UseAutoStart;
	BOOL	UseColor;
	DWORD	FaceColor;
	DWORD	FontColor;
	BOOL	UseTrnspt;
	DWORD	ForeTrnspt;
	DWORD	BackTrnspt;
	BOOL	UseHotkey;
	short	ModKey;
	short	KeyCode;
	
} SETTING, *LPSETTING;

extern SETTING Setting;

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif