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
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	Gdiplus::GdiplusShutdown(GdiPlusToken);

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}