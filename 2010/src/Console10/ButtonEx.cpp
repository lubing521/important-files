// ButtonEx.cpp : ŽÀ‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "ButtonEx.h"


// CButtonEx

CPlugButtonEx::CPlugButtonEx(UINT id, int nItem, int nSubItem, const CRect &rect, const ClickCallback &callback )
	: uID_(id)
	, m_inItem(nItem)
	, m_inSubItem(nSubItem)
	, m_rect(rect)
	, bEnable(TRUE)
	, clickCallback_(callback)
{}

CPlugButtonEx::~CPlugButtonEx()
{
}


BEGIN_MESSAGE_MAP(CPlugButtonEx, CMFCButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CPlugButtonEx::OnBnClicked)
END_MESSAGE_MAP()






void CPlugButtonEx::OnBnClicked()
{
	if( clickCallback_ != NULL )
		clickCallback_(uID_, m_inItem, m_inSubItem);
}
