#include "stdafx.h"
#include "Quicker.h"
#include "QuickerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CQuickerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CQuickerApp::CQuickerApp()
{

}

CQuickerApp theApp;
ULONG_PTR GdiPlusToken;

#define MUTEX_GUID_RELEASE	_T("{6050719D-61DC-43a0-931C-D73C22539423}")
#define MUTEX_GUID_DEBUG	_T("{9629DE60-23A6-473b-B290-D7D13917F671}")


BOOL CQuickerApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

#ifdef _DEBUG
	HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_GUID_DEBUG);
#else
	HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_GUID_RELEASE);
#endif
	
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		return FALSE;
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	AfxInitRichEdit2();
	SetRegistryKey(_T("zextor"));

	CQuickerDlg dlg;
	m_pMainWnd = &dlg;

	Gdiplus::GdiplusStartupInput GdiPlusStartupInput;
	Gdiplus::GdiplusStartup(&GdiPlusToken, &GdiPlusStartupInput, NULL);
	

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	Gdiplus::GdiplusShutdown(GdiPlusToken);

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}