// ToolbarCompCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Quicker.h"
#include "ToolbarCompCtrl.h"


// CToolbarCompCtrl

IMPLEMENT_DYNAMIC(CToolbarCompCtrl, CToolBarCtrl)

CToolbarCompCtrl::CToolbarCompCtrl()
{

}

CToolbarCompCtrl::~CToolbarCompCtrl()
{
}


BEGIN_MESSAGE_MAP(CToolbarCompCtrl, CToolBarCtrl)
	ON_WM_MOVE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CToolbarCompCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CToolbarCompCtrl �޽��� ó�����Դϴ�.



void CToolbarCompCtrl::OnMove(int x, int y)
{
	CToolBarCtrl::OnMove(x, y);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CToolbarCompCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}
