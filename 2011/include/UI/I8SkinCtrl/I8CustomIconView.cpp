#include "StdAfx.h"
#include "I8CustomIconView.h"

using namespace I8SkinCtrl_ns;

CI8CustomIconView::CI8IconItem::CI8IconItem(ResData_st& stResData)
    : m_stResData(stResData)
{
}

CI8CustomIconView::CI8IconItem::~CI8IconItem(void)
{
}

void CI8CustomIconView::CI8IconItem::OnDraw(CDC* pDC, const CI8ItemList::ItemInfo_st& stItemInfo)
{
    if (m_stResData.phIcon[stItemInfo.iIndex] == NULL)
    {
        m_stResData.m_pfnRequestIcon(stItemInfo.iIndex);
    }
    if (m_stResData.phIcon[stItemInfo.iIndex] != NULL)
    {
        pDC->DrawIcon(stItemInfo.rtRect.left, stItemInfo.rtRect.top, m_stResData.phIcon[stItemInfo.iIndex]);
    }
    else
    {
        pDC->FillSolidRect(stItemInfo.rtRect, RGB(255, 0, 0));
    }
}

CI8CustomIconView::CI8CustomIconView(CI8CtrlBackground* pclBackground)
    : CI8SimpleIconView(FALSE, pclBackground)
    , m_clSeeIcon(m_stResData)
{
}

void CI8CustomIconView::SetIcon(HICON* phIcon, int iCount)
{
    assert(phIcon != NULL && iCount >= 0);
    m_stResData.phIcon = phIcon;
    SetItemCount(iCount, &m_clSeeIcon);
}

void CI8CustomIconView::Update()
{
    NeedRedraw();
    Invalidate();
}

void CI8CustomIconView::SetDrawCallBack(std::tr1::function<void(int iIconIndex)> pfnRequestIcon)
{
    m_stResData.m_pfnRequestIcon = pfnRequestIcon;
}
