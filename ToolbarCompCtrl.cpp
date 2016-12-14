// ToolbarCompCtrl.cpp : 구현 파일입니다.
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



// CToolbarCompCtrl 메시지 처리기입니다.



void CToolbarCompCtrl::OnMove(int x, int y)
{
	CToolBarCtrl::OnMove(x, y);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CToolbarCompCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
