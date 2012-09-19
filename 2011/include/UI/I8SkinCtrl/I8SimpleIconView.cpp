#include "stdafx.h"
#include "I8SimpleIconView.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8SimpleIconView, CI8ItemList)

BEGIN_MESSAGE_MAP(CI8SimpleIconView, CI8ItemList)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

CI8SimpleIconView::CI8CalculateIconItemRect::CI8CalculateIconItemRect(CI8SimpleIconView* pclIconView)
    : m_pclIconView(pclIconView)
{
}

CI8SimpleIconView::CI8CalculateIconItemRect::~CI8CalculateIconItemRect()
{
}

void CI8SimpleIconView::CI8CalculateIconItemRect::Begin(const CRect& rtClient)
{
    m_iColumnCount = max((rtClient.Width() + m_pclIconView->m_iItemHoriSpace) / (m_pclIconView->m_clItemSize.cx + m_pclIconView->m_iItemHoriSpace), 1);
    m_iRowCount= max((rtClient.Height() + m_pclIconView->m_iItemVertSpace) / (m_pclIconView->m_clItemSize.cy + m_pclIconView->m_iItemVertSpace), 1);
    m_iItemCountEveryPage = m_iColumnCount * m_iRowCount;
    int iTotalRowCount = max((m_pclIconView->GetItemCount() + m_iColumnCount - 1) / m_iColumnCount, 0);
    m_pclIconView->m_iVertTotalLength = max(iTotalRowCount * (m_pclIconView->m_clItemSize.cy + m_pclIconView->m_iItemVertSpace) - m_pclIconView->m_iItemVertSpace, rtClient.Height());
    m_pclIconView->m_iVertPageLength = rtClient.Height();
    m_pclIconView->m_iCurVertPos = min(m_pclIconView->m_iCurVertPos, max(m_pclIconView->m_iVertTotalLength - rtClient.Height(), 0));
    m_iLeft = rtClient.left;
    m_iRight = rtClient.left;
    m_iValidItemBeginIndex = (m_pclIconView->m_iCurVertPos + 1) / (m_pclIconView->m_clItemSize.cy + m_pclIconView->m_iItemVertSpace) * m_iColumnCount;
    m_iValidItemEndIndex = (m_pclIconView->m_iCurVertPos + rtClient.Height() - 1) / (m_pclIconView->m_clItemSize.cy + m_pclIconView->m_iItemVertSpace) * m_iColumnCount + m_iColumnCount - 1;
}

void CI8SimpleIconView::CI8CalculateIconItemRect::CalculateItem(int iIndex, CI8ItemList::CI8Item* pclItem, CRect& rtItem, BOOL& bVisible)
{
    if (iIndex >= m_iValidItemBeginIndex && iIndex <= m_iValidItemEndIndex)
    {
        int iX = m_iLeft + (iIndex % m_iColumnCount) * (m_pclIconView->m_clItemSize.cx + m_pclIconView->m_iItemHoriSpace);
        int iY = m_iRight + (iIndex / m_iColumnCount) * (m_pclIconView->m_clItemSize.cy + m_pclIconView->m_iItemVertSpace) - m_pclIconView->m_iCurVertPos;
        rtItem = CRect(CPoint(iX, iY), m_pclIconView->m_clItemSize);
        bVisible = TRUE;
    }
    else
    {
        bVisible = FALSE;
    }
}

CI8SimpleIconView::CI8SimpleIconView(BOOL bAutoFreeItem, CI8CtrlBackground* pclBackground)
    : CI8ItemList(m_clCalculateIconItemRect, bAutoFreeItem, pclBackground)
    , m_clCalculateIconItemRect(this)
    , m_clItemSize(32, 32)
    , m_iItemHoriSpace(5)
    , m_iItemVertSpace(5)
    , m_iVertTotalLength(0)
    , m_iVertPageLength(0)
    , m_iCurVertPos(0)
    , m_pVertScroll(NULL)
{
}

CI8SimpleIconView::~CI8SimpleIconView()
{
}

void CI8SimpleIconView::SetItemSize(const CSize& clItemSize, const CSize& clItemSpace)
{
    assert(clItemSize.cx > 0 && clItemSize.cy > 0);
    assert(clItemSpace.cx >= 0 && clItemSpace.cy >= 0);
    m_clItemSize = clItemSize;
    m_iItemHoriSpace = clItemSpace.cx;
    m_iItemVertSpace = clItemSpace.cy;
    if (GetSafeHwnd() != NULL)
    {
        UpdateItemRect();
    }
}

void CI8SimpleIconView::SetVertPos(int iPos)
{
    if (iPos != m_iCurVertPos)
    {
        if (iPos < 0)
        {
            m_iCurVertPos = iPos;
        }
        else
        {
            if (iPos > m_iVertTotalLength - m_iVertPageLength)
            {
                m_iCurVertPos = m_iVertTotalLength - m_iVertPageLength;
            }
            else
            {
                m_iCurVertPos = iPos;
            }
        }
        if (GetSafeHwnd() != NULL)
        {
            UpdateItemRect();
        }
    }
}

int CI8SimpleIconView::GetCurVertPos() const
{
    return m_iCurVertPos;
}

CScrollBar* CI8SimpleIconView::ApplyVertScroll()
{
    return new CScrollBar;
}

void CI8SimpleIconView::OnSize(IN UINT nType, IN int cx, IN int cy)
{
    if (m_pVertScroll != NULL)
    {
        CRect rtClient;
        ::GetClientRect(GetSafeHwnd(), &rtClient);
        CRect rtVertScrollRect = CRect(rtClient.right - GetSystemMetrics(SM_CYVSCROLL), rtClient.top, rtClient.right, rtClient.bottom);
        m_pVertScroll->MoveWindow(rtVertScrollRect);
    }
    __super::OnSize(nType, cx, cy);
}

void CI8SimpleIconView::OnUpdateItemRect()
{
    __super::OnUpdateItemRect();
    if (m_pVertScroll != NULL)
    {
        if (m_iVertPageLength >= m_iVertTotalLength)
        {
            m_pVertScroll->EnableWindow(FALSE);
        }
        else
        {
            m_pVertScroll->EnableWindow();
            SCROLLINFO stScrollInfo;
            memset(&stScrollInfo, 0, sizeof(SCROLLINFO));
            stScrollInfo.cbSize = sizeof(SCROLLINFO);
            stScrollInfo.fMask = SIF_ALL;
            stScrollInfo.nMin = 0;
            stScrollInfo.nMax = m_iVertTotalLength - 1;
            stScrollInfo.nPage = m_iVertPageLength;
            stScrollInfo.nPos = m_iCurVertPos;
            stScrollInfo.nTrackPos = 0;
            m_pVertScroll->SetScrollInfo(&stScrollInfo, TRUE);
        }
    }
}

void CI8SimpleIconView::Init()
{
    __super::Init();
    m_pVertScroll = ApplyVertScroll();
    if (m_pVertScroll != NULL)
    {
        m_pVertScroll->Create(WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | SBS_VERT, CRect(0, 0, 0, 0), this, 0);
    }
}

void CI8SimpleIconView::OnDestroy()
{
    __super::OnDestroy();
    if (m_pVertScroll != NULL)
    {
        delete m_pVertScroll;
        m_pVertScroll = NULL;
    }
}

CRect CI8SimpleIconView::GetItemArea(const CRect& rtRect) const
{
    CRect rtOutRect = rtRect;
    if (m_pVertScroll != NULL)
    {
        rtOutRect.right -= GetSystemMetrics(SM_CYVSCROLL);
    }
    return rtOutRect;
}

void CI8SimpleIconView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pVScroll)
{
    int iPos = m_iCurVertPos;
    switch(nSBCode)
    {
    case SB_LINEUP:
        --iPos;
        break;
    case SB_LINEDOWN:
        ++iPos;
        break;
    case SB_PAGEUP:
        iPos -= m_iVertPageLength;
        break;
    case SB_PAGEDOWN:
        iPos += m_iVertPageLength;
        break;
    case SB_THUMBTRACK:
        iPos = nPos;
        break;
    }
    iPos = min(max(iPos, 0), m_iVertTotalLength - m_iVertPageLength);
    if (iPos != m_iCurVertPos)
    {
        m_iCurVertPos = iPos;
        UpdateItemRect();
    }
}
