#include "stdafx.h"
#include "I8ItemList.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8ItemList, CWnd)

BEGIN_MESSAGE_MAP(CI8ItemList, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CI8ItemList::CI8ItemList(CI8CalculateItemRect& clCalculateItemRect, BOOL bAutoFreeItem, CI8CtrlBackground* pclBackground)
    : m_clCalculateItemRect(clCalculateItemRect)
    , m_bMouseHover(FALSE)
    , m_bMulSelect(FALSE)
    , m_bDrawReverse(FALSE)
    , m_bAutoFreeItem(bAutoFreeItem)
    , m_pclBackground(pclBackground)
{
    m_clCurItem = m_clItems.end();
}

CI8ItemList::~CI8ItemList()
{
}

int CI8ItemList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    BOOL bResult = __super::OnCreate(lpCreateStruct);
    if (bResult != -1)
    {
        Init();
    }
    return bResult;
}

void CI8ItemList::Init()
{
}

void CI8ItemList::OnDestroy()
{
    Clear();
    __super::OnDestroy();
}

BOOL CI8ItemList::OnEraseBkgnd(CDC* pDC)
{
    if (m_clBGCache.IsNeedRedraw())
    {
        CI8Bitmap& clImage = m_clBGCache.GetImage();
        CI8Bitmap::CI8Draw clDraw;
        HDC hDC = clImage.GetDC(clDraw, *pDC);
        CDC* pCachDC = CDC::FromHandle(hDC);
        CRect rtClient;
        __super::GetClientRect(&rtClient);
        if (m_pclBackground == NULL)
        {
            pCachDC->FillSolidRect(&rtClient, GetSysColor(COLOR_MENU));
        }
        else
        {
            m_pclBackground->OnDraw(this, pCachDC, GetItemArea(rtClient));
        }
        CRect rtItemValidArea = GetClientRect();
        CRgn clItemRgn;
        clItemRgn.CreateRectRgn(rtItemValidArea.left, rtItemValidArea.top, rtItemValidArea.right, rtItemValidArea.bottom);
        pCachDC->SelectClipRgn(&clItemRgn, RGN_COPY);
        DrawAllItem(pCachDC);
    }
    m_clBGCache.Draw(*pDC);
    return TRUE;
}

void CI8ItemList::DrawAllItem(CDC* pDC)
{
    if (m_bDrawReverse)
    {
        for (std::list<std::list<ItemData_st>::iterator>::reverse_iterator clId = m_clVisibleItems.rbegin(); clId != m_clVisibleItems.rend(); ++clId)
        {
            ItemData_st& stItemData = *(*clId);
            if (!stItemData.bSelected)
            {
                ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == *clId};
                stItemData.pclItem->OnDraw(pDC, stItemInfo);
            }
        }
    }
    else
    {
        for (std::list<std::list<ItemData_st>::iterator>::iterator clId = m_clVisibleItems.begin(); clId != m_clVisibleItems.end(); ++clId)
        {
            ItemData_st& stItemData = *(*clId);
            if (!stItemData.bSelected)
            {
                ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == *clId};
                stItemData.pclItem->OnDraw(pDC, stItemInfo);
            }
        }
    }
    for (std::list<std::list<ItemData_st>::iterator>::iterator clId = m_clSelectedItems.begin(); clId != m_clSelectedItems.end(); ++clId)
    {
        ItemData_st& stItemData = *(*clId);
        if (stItemData.bVisible)
        {
            ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == *clId};
            stItemData.pclItem->OnDraw(pDC, stItemInfo);
        }
    }
}

void CI8ItemList::OnSize(IN UINT nType, IN int cx, IN int cy)
{
    m_clBGCache.SetSize(CSize(cx, cy));
    OnUpdateItemRect();
    return __super::OnSize(nType, cx, cy);
}

void CI8ItemList::OnMouseMove(UINT nFlags, CPoint point)
{
    if(!m_bMouseHover) 
    {
        TRACKMOUSEEVENT tme = { 0 };
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        _TrackMouseEvent(&tme);

        m_bMouseHover = TRUE;
    }
    std::list<ItemData_st>::iterator clId = GetItemByPos(point);
    if (clId != m_clCurItem)
    {
        m_clCurItem = clId;
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}

void CI8ItemList::OnMouseLeave()
{
    if(m_bMouseHover) 
    {
        m_bMouseHover = FALSE;
    }
    std::list<ItemData_st>::iterator clId = m_clItems.end();
    if (clId != m_clCurItem)
    {
        m_clCurItem = clId;
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}

void CI8ItemList::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_clCurItem != m_clItems.end())
    {
        ItemData_st& stItemData = *m_clCurItem;
        if (m_bMulSelect && (nFlags & MK_SHIFT))
        {
            SetSelected(m_clCurItem, !(stItemData.bSelected));
        }
        else
        {
            CancelSelect(TRUE);
            SetSelected(m_clCurItem, TRUE);
        }
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, TRUE};
        stItemData.pclItem->OnLButtonDown(nFlags, point, stItemInfo);
    }
}

void CI8ItemList::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_clCurItem != m_clItems.end())
    {
        ItemData_st& stItemData = *m_clCurItem;
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, TRUE};
        stItemData.pclItem->OnLButtonUp(nFlags, point, stItemInfo);
    }
}

void CI8ItemList::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (m_clCurItem != m_clItems.end())
    {
        ItemData_st& stItemData = *m_clCurItem;
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, TRUE};
        stItemData.pclItem->OnRButtonDown(nFlags, point, stItemInfo);
    }
}

void CI8ItemList::OnRButtonUp(UINT nFlags, CPoint point)
{
    if (m_clCurItem != m_clItems.end())
    {
        ItemData_st& stItemData = *m_clCurItem;
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, TRUE};
        stItemData.pclItem->OnRButtonUp(nFlags, point, stItemInfo);
    }
}

void CI8ItemList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (m_clCurItem != m_clItems.end())
    {
        ItemData_st& stItemData = *m_clCurItem;
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, TRUE};
        stItemData.pclItem->OnLButtonDblClk(nFlags, point, stItemInfo);
    }
}

std::list<CI8ItemList::ItemData_st>::iterator CI8ItemList::GetItemByPos(const CPoint& clPos)
{
    if (m_clCurItem != m_clItems.end())
    {
        if (m_clCurItem->rtItem.PtInRect(clPos))
        {
            return m_clCurItem;
        }
    }
    for (std::list<std::list<ItemData_st>::iterator>::iterator clId = m_clVisibleItems.begin(); clId != m_clVisibleItems.end(); ++clId)
    {
        if ((*clId)->rtItem.PtInRect(clPos))
        {
            return *clId;
        }
    }
    return m_clItems.end();
}

void CI8ItemList::SetSelected(const std::list<ItemData_st>::iterator& clItem, BOOL bSelected, BOOL bNotify)
{
    assert(clItem != m_clItems.end());
    assert((m_clSelectedItems.size() == 0 || !bSelected || !m_bMulSelect) && "不支持多选");
    ItemData_st& stItemData = *clItem;
    if (bSelected)
    {
        if (!stItemData.bSelected)
        {
            stItemData.bSelected = TRUE;
            m_clSelectedItems.push_back(clItem);
            if (bNotify)
            {
                ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == clItem};
                clItem->pclItem->OnSelect(stItemInfo);
            }
        }
    }
    else
    {
        if (stItemData.bSelected)
        {
            stItemData.bSelected = FALSE;
            m_clSelectedItems.remove(clItem);
            if (bNotify)
            {
                ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == clItem};
                clItem->pclItem->OnUnselect(stItemInfo);
            }
        }
    }
    m_clBGCache.NeedRedraw();
    Invalidate();
}

void CI8ItemList::CancelSelect(BOOL bNotify)
{
    for (std::list<std::list<ItemData_st>::iterator>::iterator clId = m_clSelectedItems.begin(); clId != m_clSelectedItems.end(); ++clId)
    {
        ItemData_st& stItemData = **clId;
        stItemData.bSelected = FALSE;
        if (bNotify)
        {
            ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == *clId};
            stItemData.pclItem->OnUnselect(stItemInfo);
        }
    }
    m_clSelectedItems.clear();
}

void CI8ItemList::CancelSelect()
{
    CancelSelect(TRUE);
    m_clBGCache.NeedRedraw();
    Invalidate();
}

void CI8ItemList::OnUpdateItemRect()
{
    m_clVisibleItems.clear();
    CRect rtClient = GetClientRect();
    m_clCalculateItemRect.Begin(rtClient);
    int iIndex = 0;
    for (std::list<ItemData_st>::iterator clId = m_clItems.begin(); clId != m_clItems.end(); ++clId)
    {
        ItemData_st& stItemData = *clId;
        stItemData.iIndex = iIndex;
        m_clCalculateItemRect.CalculateItem(iIndex, stItemData.pclItem, stItemData.rtItem, stItemData.bVisible);
        if (stItemData.bVisible)
        {
            m_clVisibleItems.push_back(clId);
        }
        ++iIndex;
    }
    m_clCalculateItemRect.End();
}

void I8SkinCtrl_ns::CI8ItemList::UpdateItemRect(BOOL bRedraw)
{
    if (GetSafeHwnd() != NULL)
    {
        OnUpdateItemRect();
        m_clBGCache.NeedRedraw();
        if (bRedraw)
        {
            Invalidate();
        }
    }
}

CRect CI8ItemList::GetClientRect() const
{
    CRect rtClient;
    __super::GetClientRect(rtClient);
    if (m_pclBackground == NULL)
    {
        return GetItemArea(rtClient);
    }
    else
    {
        return m_pclBackground->GetValidRect(GetItemArea(rtClient));
    }
}

CRect I8SkinCtrl_ns::CI8ItemList::GetItemArea(const CRect& rtRect) const
{
    return rtRect;
}

void CI8ItemList::Create(DWORD dwStyle, const CRect& rtRect, CWnd* pclWnd, UINT nID)
{
    dwStyle |= WS_CLIPCHILDREN;
    __super::Create(NULL, NULL, dwStyle, rtRect, pclWnd, nID);
}

void CI8ItemList::Clear()
{
    m_clSelectedItems.clear();
    m_clVisibleItems.clear();
    m_clCurItem = m_clItems.end();
    for (std::list<ItemData_st>::iterator clId = m_clItems.begin(); clId != m_clItems.end(); ++clId)
    {
        ItemData_st& stItemData = *clId;
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == clId};
        stItemData.pclItem->OnDelete(stItemInfo);
        if (m_bAutoFreeItem)
        {
            delete clId->pclItem;
        }
    }
    m_clItems.clear();
}

void CI8ItemList::SetSelected(int iIndex, BOOL bSelected, BOOL bNotify)
{
    assert(iIndex >= 0 && iIndex < int(m_clItems.size()));
    SetSelected(GetItemFromIndex(iIndex), bSelected, bNotify);
}

void CI8ItemList::Select(int iIndex, BOOL bNotify)
{
    assert(iIndex >= 0 && iIndex < int(m_clItems.size()));
    CancelSelect(bNotify);
    SetSelected(GetItemFromIndex(iIndex), TRUE, bNotify);
}

void CI8ItemList::AddItem(CI8Item* pclItem, BOOL bUpdate)
{
    assert(pclItem != NULL && "空指针");
    ItemData_st stAddItemData = {pclItem, CRect(0, 0, 0, 0), -1, FALSE, FALSE};
    m_clItems.push_back(stAddItemData);
    if (bUpdate)
    {
        UpdateItemRect();
    }
    ItemData_st& stItemData = *m_clItems.rbegin();
    ItemInfo_st stItemInfo = {this, m_clItems.size() - 1, stItemData.rtItem, stItemData.bSelected, m_clCurItem != m_clItems.end() && (&(*m_clCurItem) == &(*m_clItems.rbegin()))};
    stItemData.pclItem->OnAdd(stItemInfo);
}

void CI8ItemList::SetItemCount(int iCount, CI8Item* pclItem)
{
    assert(pclItem != NULL && "空指针");
    assert(iCount >= 0 && "错误的数量");
    assert(!m_bAutoFreeItem && "不能在自动释放模式下使用SetItemCount函数");
    ItemData_st stAddItemData = {pclItem, CRect(0, 0, 0, 0), -1, FALSE, FALSE};
    for (int i = 0; i < iCount; ++i)
    {
        m_clItems.push_back(stAddItemData);
    }
    UpdateItemRect();
    if (iCount > 0)
    {
        ItemData_st& stItemData = *m_clItems.rbegin();
        ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem != m_clItems.end() && (&(*m_clCurItem) == &(*m_clItems.rbegin()))};
        stItemData.pclItem->OnAdd(stItemInfo);
    }
}

BOOL CI8ItemList::IsSelected(int iIndex) const
{
    assert(iIndex >= 0 && iIndex < int(m_clItems.size()));
    for (std::list<std::list<ItemData_st>::iterator>::const_iterator clId = m_clSelectedItems.begin(); clId != m_clSelectedItems.end(); ++clId)
    {
        if ((*clId)->iIndex == iIndex)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CI8ItemList::IsSelected() const
{
    return m_clSelectedItems.size() != 0;
}

int CI8ItemList::GetSelected() const
{
    assert(m_clSelectedItems.size() <= 1 && "图标列表的函数GetSelected只能在只有一个物体被选中时使用");
    if (m_clSelectedItems.size() == 0)
    {
        return -1;
    }
    else
    {
        return (*m_clSelectedItems.begin())->iIndex;
    }
}

std::list<CI8ItemList::ItemData_st>::iterator CI8ItemList::GetItemFromIndex(int iIndex)
{
    int iItemIndex = 0;
    for (std::list<ItemData_st>::iterator clId = m_clItems.begin(); clId != m_clItems.end(); ++clId)
    {
        if (iItemIndex == iIndex)
        {
            return clId;
        }
        ++iItemIndex;
    }
    assert(0 && "错误的索引");
    return m_clItems.end();
}

void CI8ItemList::DeleteItem(int iIndex, BOOL bUpdate)
{
    assert(iIndex >= 0 && iIndex < int(m_clItems.size()));
    for (std::list<ItemData_st>::iterator clId = m_clItems.begin(); clId != m_clItems.end(); ++clId)
    {
        if (clId->iIndex == iIndex)
        {
            if (clId == m_clCurItem)
            {
                m_clCurItem = m_clItems.end();
            }
            if (clId->bSelected)
            {
                m_clSelectedItems.remove(clId);
            }
            if (clId->bVisible)
            {
                m_clVisibleItems.remove(clId);
            }
            ItemData_st& stItemData = *clId;
            ItemInfo_st stItemInfo = {this, stItemData.iIndex, stItemData.rtItem, stItemData.bSelected, m_clCurItem == clId};
            stItemData.pclItem->OnDelete(stItemInfo);
            if (m_bAutoFreeItem)
            {
                delete clId->pclItem;
            }
            m_clItems.erase(clId);
            if (bUpdate)
            {
                UpdateItemRect();
            }
            stItemData.pclItem->OnDeleted(stItemInfo);
            return;
        }
    }
}

void CI8ItemList::NeedRedraw()
{
    m_clBGCache.NeedRedraw();
}

int CI8ItemList::GetItemCount() const
{
    return m_clItems.size();
}

int CI8ItemList::GetSelectedItemCount() const
{
    return m_clSelectedItems.size();
}

CI8ItemList::CI8Item* CI8ItemList::GetItem(int iIndex)
{
    return GetItemFromIndex(iIndex)->pclItem;
}

void CI8ItemList::SetDrawReverse(BOOL bDrawReverse)
{
    m_bDrawReverse = bDrawReverse;
    if (GetSafeHwnd() != NULL)
    {
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}
