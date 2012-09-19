#include "stdafx.h"
#include "I8TabCtrl.h"
#include "I8SkinPack.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8TabCtrl, CI8ItemList)

CI8TabCtrl::CI8TabItem::CI8TabItem(ResData_st& stResData, const stdex::tString& sText, CWnd* pWnd)
    : m_stResData(stResData)
    , m_sText(sText)
    , m_pWnd(pWnd)
    , m_clItemSize(0, 0)
{
    m_clItemSize.SetSize(I8_GetTextSize(m_stResData.hFont, m_sText).cx + m_stResData.aclTabItemImage[0].GetLeftSpace() + m_stResData.aclTabItemImage[0].GetRightSpace(), m_stResData.aclTabItemImage[0].GetSize().cy);
}

CI8TabCtrl::CI8TabItem::~CI8TabItem()
{

}

void CI8TabCtrl::CI8TabItem::OnDraw(CDC* pDC, const ItemInfo_st& stItemInfo)
{
    assert(pDC != NULL);
    TAB_ITEM_STATE_EN enState = GetState(stItemInfo);
    m_stResData.aclTabItemImage[enState].Draw(*pDC, stItemInfo.rtRect);
    HFONT hOldFont = static_cast<HFONT>(pDC->SelectObject(m_stResData.hFont));
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_stResData.anFontColor[enState]);
    CRect rtText = m_stResData.aclTabItemImage[enState].GetSpaceArea(stItemInfo.rtRect);
    rtText.top += m_stResData.iFontTop;
    pDC->DrawText(m_sText.c_str(), m_sText.size(), &rtText, DT_SINGLELINE | DT_LEFT | DT_TOP);
    pDC->SelectObject(hOldFont);
}

void CI8TabCtrl::CI8TabItem::OnSelect(const ItemInfo_st& stItemInfo)
{
    if (m_pWnd != NULL)
    {
        m_pWnd->MoveWindow(stItemInfo.pclWnd->GetClientRect());
        m_pWnd->ShowWindow(SW_SHOW);
    }
}

void CI8TabCtrl::CI8TabItem::OnUnselect(const ItemInfo_st& stItemInfo)
{
    if (m_pWnd != NULL)
    {
        m_pWnd->ShowWindow(SW_HIDE);
    }
}

void CI8TabCtrl::CI8TabItem::OnAdd(const ItemInfo_st& stItemInfo)
{
    if (!stItemInfo.pclWnd->IsSelected())
    {
        stItemInfo.pclWnd->Select(stItemInfo.iIndex);
    }
}

void CI8TabCtrl::CI8TabItem::OnDeleted(const ItemInfo_st& stItemInfo)
{
    if (!stItemInfo.bSelected)
    {
        if (stItemInfo.pclWnd->GetItemCount() != 0)
        {
            stItemInfo.pclWnd->Select(0);
        }
    }
}

CSize CI8TabCtrl::CI8TabItem::GetItemSize(int iIndex) const
{
    return m_clItemSize;
}

CI8TabCtrl::TAB_ITEM_STATE_EN CI8TabCtrl::CI8TabItem::GetState(const ItemInfo_st& stItemInfo) const
{
    if (stItemInfo.bHover)
    {
        if (stItemInfo.bSelected)
        {
            return E_TAB_ITEM_SELECTED_HOVER;
        }
        else
        {
            return E_TAB_ITEM_UNSELECTED_HOVER;
        }
    }
    else
    {
        if (stItemInfo.bSelected)
        {
            return E_TAB_ITEM_SELECTED;
        }
        else
        {
            return E_TAB_ITEM_UNSELECTED;
        }
    }
}

CI8TabCtrl::CI8CalculateTabItemRect::CI8CalculateTabItemRect(CI8TabCtrl* m_pclTabCtrl)
    : m_pclTabCtrl(m_pclTabCtrl)
{
}

CI8TabCtrl::CI8CalculateTabItemRect::~CI8CalculateTabItemRect()
{
}

void CI8TabCtrl::CI8CalculateTabItemRect::Begin(const CRect& rtClient)
{
    m_rtValidRect = rtClient;
    m_rtValidRect.left += m_pclTabCtrl->m_iLeftSpace;
    m_rtValidRect.right -= m_pclTabCtrl->m_iRightSpace;
}

void CI8TabCtrl::CI8CalculateTabItemRect::CalculateItem(int iIndex, CI8ItemList::CI8Item* pclItem, CRect& rtItem, BOOL& bVisible)
{
    CSize clItemSize = pclItem->GetItemSize(iIndex);
    rtItem.left = m_rtValidRect.left;
    rtItem.top = 0;
    rtItem.right = rtItem.left + clItemSize.cx;
    rtItem.bottom = clItemSize.cy;
    m_rtValidRect.left += clItemSize.cx + m_pclTabCtrl->m_iItemHoriSpace;
    if (m_rtValidRect.left > m_rtValidRect.right)
    {
        bVisible = FALSE;
    }
    else
    {
        bVisible = TRUE;
    }
}

CI8TabCtrl::CI8TabCtrl()
    : CI8ItemListB(m_clCalculateTabItemRect, TRUE)
    , m_clCalculateTabItemRect(this)
    , m_iItemHoriSpace(0)
    , m_iLeftSpace(0)
    , m_iRightSpace(0)
{
    ZeroMemory(m_stResData.anFontColor, sizeof(m_stResData.anFontColor));
    m_stResData.hFont = NULL;
    m_stResData.iFontTop = 0;
    SetDrawReverse();
}

CI8TabCtrl::~CI8TabCtrl()
{
}

void CI8TabCtrl::Load(const stdex::tString& sSkinName)
{
    m_stResData.aclTabItemImage[E_TAB_ITEM_SELECTED].Load(sSkinName + _T("_选中.png"), sSkinName, _T("前景"));
    m_stResData.aclTabItemImage[E_TAB_ITEM_UNSELECTED].Load(sSkinName + _T("_未选中.png"), sSkinName, _T("前景"));
    m_stResData.aclTabItemImage[E_TAB_ITEM_SELECTED_HOVER].Load(sSkinName + _T("_选中_鼠标经过.png"), sSkinName, _T("前景"), FALSE);
    m_stResData.aclTabItemImage[E_TAB_ITEM_UNSELECTED_HOVER].Load(sSkinName + _T("_未选中_鼠标经过.png"), sSkinName, _T("前景"), FALSE);
    if (m_stResData.aclTabItemImage[E_TAB_ITEM_SELECTED_HOVER].IsNull())
    {
        m_stResData.aclTabItemImage[E_TAB_ITEM_SELECTED_HOVER] = m_stResData.aclTabItemImage[E_TAB_ITEM_SELECTED];
    }
    if (m_stResData.aclTabItemImage[E_TAB_ITEM_UNSELECTED_HOVER].IsNull())
    {
        m_stResData.aclTabItemImage[E_TAB_ITEM_UNSELECTED_HOVER] = m_stResData.aclTabItemImage[E_TAB_ITEM_UNSELECTED];
    }
    m_iItemHoriSpace = CI8SkinPack::Instance().GetOption(_T("前景"), _T("水平间隔"), 0, sSkinName);
    m_iLeftSpace = CI8SkinPack::Instance().GetOption(_T("前景"), _T("左边距"), 0, sSkinName);
    m_iRightSpace = CI8SkinPack::Instance().GetOption(_T("前景"), _T("左边距"), 0, sSkinName);
    m_stResData.hFont = CI8SkinPack::Instance().GetFont(_T("文字"), sSkinName);
    m_stResData.anFontColor[E_TAB_ITEM_SELECTED] = CI8SkinPack::Instance().GetOption(_T("文字"), _T("选中颜色"), 0xFFFFFF, sSkinName);
    m_stResData.anFontColor[E_TAB_ITEM_UNSELECTED] = CI8SkinPack::Instance().GetOption(_T("文字"), _T("未选中颜色"), 0x000000, sSkinName);
    m_stResData.anFontColor[E_TAB_ITEM_SELECTED_HOVER] = CI8SkinPack::Instance().GetOption(_T("文字"), _T("鼠标经过选中颜色"), m_stResData.anFontColor[E_TAB_ITEM_SELECTED], sSkinName);
    m_stResData.anFontColor[E_TAB_ITEM_UNSELECTED_HOVER] = CI8SkinPack::Instance().GetOption(_T("文字"), _T("鼠标经过未选中颜色"), m_stResData.anFontColor[E_TAB_ITEM_UNSELECTED], sSkinName);
    m_stResData.iFontTop = CI8SkinPack::Instance().GetOption(_T("文字"), _T("顶边距"), 0, sSkinName);
    SetBackground(sSkinName + _T("_背景.png"), sSkinName, _T("背景"));
}

void CI8TabCtrl::AddItem(const stdex::tString& sText, CWnd* pWnd, BOOL bUpdate)
{
    if (pWnd != NULL)
    {
        pWnd->ShowWindow(SW_HIDE);
        pWnd->SetParent(this);
    }
    __super::AddItem(new CI8TabItem(m_stResData, sText, pWnd), bUpdate);
}


