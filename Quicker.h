
// Quicker.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CQuickerApp:
// �� Ŭ������ ������ ���ؼ��� Quicker.cpp�� �����Ͻʽÿ�.
//

class CQuickerApp : public CWinApp
{
public:
	CQuickerApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CQuickerApp theApp;