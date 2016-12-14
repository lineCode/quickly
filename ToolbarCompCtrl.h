#pragma once


// CToolbarCompCtrl

class CToolbarCompCtrl : public CToolBarCtrl
{
	DECLARE_DYNAMIC(CToolbarCompCtrl)

public:
	CToolbarCompCtrl();
	virtual ~CToolbarCompCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


