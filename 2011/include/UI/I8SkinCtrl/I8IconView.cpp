#include "stdafx.h"
#include "I8IconView.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8IconView, CListCtrl)

BEGIN_MESSAGE_MAP(CI8IconView, CListCtrl)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    //ON_WM_NCPAINT()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
END_MESSAGE_MAP()

const UINT C_DEFEAT_BG_COLOR = RGB(255, 255, 255);

CI8IconView::CI8IconView()
    : m_bCreate(FALSE)
    , m_bBGBitmapStretch(TRUE)
    , m_nBGColor(C_DEFEAT_BG_COLOR)
    , m_iIconHoriSpace(0)
{
}

BOOL CI8IconView::PreCreateWindow(CREATESTRUCT& cs)
{
    m_bCreate = TRUE;
    return __super::PreCreateWindow(cs);
}

int CI8IconView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    BOOL bResult = __super::OnCreate(lpCreateStruct);
    if (bResult != -1)
    {
        Init();
    }
    return bResult;
}

void CI8IconView::PreSubclassWindow()
{
    __super::PreSubclassWindow();
    if (!m_bCreate)
    {
        Init();
    }
}

void CI8IconView::Init()
{

}

void CI8IconView::OnDestroy()
{
    __super::OnDestroy();
}

BOOL CI8IconView::OnEraseBkgnd(CDC* pDC)
{;
    CRect rtWnd;
    CRect rtClient;
    GetClientRect(&rtClient);
    if (m_clBGBitmap.IsNull())
    {
        pDC->FillSolidRect(rtClient, m_nBGColor);
    }
    else
    {
        if (m_bBGBitmapStretch)
        {
            assert(0);
            //XXXXm_clBGBitmap.Fill(*pDC, rtClient);
        }
        else
        {
            m_clBGBitmap.Draw(*pDC, rtClient);
        }
    }
    return TRUE;
}

void CI8IconView::OnNcPaint()
{
//     CDC* pDC = GetWindowDC();
//     CRect rtWnd;
//     GetWindowRect(&rtWnd);
//     rtWnd.MoveToXY(0, 0);
//     pDC->FillSolidRect(rtWnd, 0x0000FF);
}

void CI8IconView::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    assert(pNMHDR != NULL && pResult != NULL);
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    switch (pNMCD->dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;
    case CDDS_ITEMPREPAINT:
        {
            CRect rtItem;
            GetItemRect(pNMCD->dwItemSpec, rtItem, LVIR_BOUNDS);
            if (m_iIconHoriSpace > 0)
            {
                int iLeftHortSpace = m_iIconHoriSpace / 2;
                rtItem.left += iLeftHortSpace;
                rtItem.right -= m_iIconHoriSpace - iLeftHortSpace;
            }
            ItemData_st* pstItemData = reinterpret_cast<ItemData_st*>(pNMCD->lItemlParam);
            assert(pstItemData != NULL);
            DWORD dwType = GetStyle() & LVS_TYPEMASK;
            switch (dwType)
            {
            case LVS_ICON:
                if (pstItemData->pclIconItem == NULL)
                {
                    assert(0);
                }
                pstItemData->pclIconItem->Set(this, IsItemSelected(pNMCD->dwItemSpec));
                pstItemData->pclIconItem->OnDraw(CDC::FromHandle(pNMCD->hdc), pNMCD->dwItemSpec, rtItem);
                break;
            default:
                break;
            }
        }
        *pResult = CDRF_SKIPDEFAULT;
        break;
    default:
        *pResult = CDRF_DODEFAULT;
        break;
    }
}


void I8SkinCtrl_ns::CI8IconView::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    assert(pNMHDR != NULL && pResult != NULL);
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    ItemData_st* pstItemData = reinterpret_cast<ItemData_st*>(pNMLV->lParam);
    assert(pstItemData != NULL);
    delete pstItemData;
    OnDeleteItem(pNMLV->iItem);
    *pResult = 0;
}

void CI8IconView::OnDeleteItem(int iIndex)
{
}

void CI8IconView::SetBackground(const stdex::tString& sImagePath, BOOL bStretch)
{
    assert(m_nBGColor == C_DEFEAT_BG_COLOR);
    CI8SkinPack::Instance().LoadSkin(m_clBGBitmap, sImagePath);
    m_bBGBitmapStretch = bStretch;
}

void I8SkinCtrl_ns::CI8IconView::SetBackground(UINT nBGColor)
{
    assert(m_clBGBitmap.IsNull());
    m_nBGColor = nBGColor;
}

BOOL CI8IconView::IsItemSelected(int iIndex) const
{
    if (GetSelectedCount() != 0)
    {
        POSITION pPos = GetFirstSelectedItemPosition();
        INT32 i = 0;
        while (pPos != NULL)
        {
            if (iIndex == GetNextSelectedItem(pPos))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

int CI8IconView::AddItem(CI8IconItem* pclItem)
{
    assert(pclItem != NULL);
    ItemData_st* pstItemData = new ItemData_st;
    pstItemData->pclIconItem = pclItem;
    int iIndex = __super::InsertItem(LVIF_IMAGE | LVIF_PARAM, GetItemCount(), NULL, 0, 0, 0, reinterpret_cast<LPARAM>(pstItemData));
    assert(iIndex != -1);
    return iIndex;
}

void CI8IconView::SetSelected(int iIndex, BOOL bSelected)
{
    if (bSelected)
    {
        SetItemState(iIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }
    else
    {
        SetItemState(iIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

void CI8IconView::SetItemSize(const CSize& clItemSize, const CSize& clItemSpace)
{
    assert(clItemSize.cx > 0 && clItemSize.cy > 0);
    assert(clItemSpace.cx >= 0 && clItemSpace.cy >= 0 );
    const int iFontHeight = 8;
    m_clFont.CreateFont(iFontHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
    SetFont(&m_clFont);
    m_clImageList.Create(clItemSize.cx, clItemSize.cy - iFontHeight - 8, ILC_COLOR32, 1, 0);
    SetImageList(&m_clImageList, LVSIL_NORMAL);
    SetIconSpacing(clItemSize.cx + clItemSpace.cx, clItemSize.cy + clItemSpace.cy);
    m_iIconHoriSpace = clItemSpace.cy;
}
