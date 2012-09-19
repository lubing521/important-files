// LabelWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "I8SkinPack.h"
#include "I8LabelWnd.h"

using namespace I8SkinCtrl_ns;

LPCTSTR CI8LabelWnd::ms_ptClassName = NULL; //注册窗口类名
INT32   CI8LabelWnd::ms_iRefCount   = 0;    //引用计数

IMPLEMENT_DYNAMIC(CI8LabelWnd, CWnd)

BEGIN_MESSAGE_MAP(CI8LabelWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

CI8LabelWnd::CI8LabelWnd()
    : m_bUpdateRgn(FALSE)
    , m_bDynamic(FALSE)
    , m_enDefaultDirection(E_DN_LEFT)
    , m_iDesPointInterval(0)
{
    ++ms_iRefCount;
    if (ms_iRefCount == 1)
    {
        ms_ptClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, NULL, NULL);
    }
    m_rtClient.SetRect(0, 0, 0, 0);
    m_rtEdge.SetRect(0, 0, 0, 0);
    m_clDesPos.SetPoint(0, 0);
    m_enDirection = E_DN_LEFT;
    m_clArrowPos.SetPoint(0, 0);
}

CI8LabelWnd::~CI8LabelWnd()
{
    if (ms_iRefCount == 1)
    {
        UnregisterClass(ms_ptClassName, AfxGetInstanceHandle());
    }
    --ms_iRefCount;
}

void CI8LabelWnd::LoadSkin(const stdex::tString sName)
{
    m_clBGImage.Load(sName + _T("/BG"));
    CI8SkinPack::Instance().LoadSkin(m_aclArrowImage[E_DN_LEFT], sName + _T("/Arrow_Left.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclArrowImage[E_DN_RIGHT], sName + _T("/Arrow_Right.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclArrowImage[E_DN_TOP], sName + _T("/Arrow_Top.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclArrowImage[E_DN_BOTTOM], sName + _T("/Arrow_Bottom.png"));
}

int CI8LabelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CI8LabelWnd::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    if (cx != 0 && cy != 0)
    {
        UpdateSize();
    }
}

BOOL CI8LabelWnd::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CI8LabelWnd::OnPaint()
{
    CPaintDC dc(this);
    if (m_clBGCache.IsNeedRedraw())
    {
        CI8Bitmap::CI8Draw clDraw;
        HDC hMemDC = m_clBGCache.GetImage().GetDC(clDraw, dc);
        DrawEgde(hMemDC);
        if (m_bUpdateRgn)
        {
            m_bUpdateRgn = FALSE;
            m_clBGCache.UpdateRgn();
            SetWindowRgn(m_clBGCache.GetRgn(), TRUE);
        }
        CRgn clRgn;
        clRgn.CreateRectRgn(m_rtClient.left, m_rtClient.top, m_rtClient.right, m_rtClient.bottom);
        SelectClipRgn(hMemDC, clRgn);
        OnDraw(hMemDC, m_rtClient);
        SelectClipRgn(hMemDC, NULL);
    }
    if (!m_clBGCache.IsNull())
    {
        m_clBGCache.Draw(dc);
    }
}

void CI8LabelWnd::UpdateSize()
{
    CRect rtClient;
    GetClientRect(&rtClient);
    m_clBGCache.SetSize(rtClient.Size());
    CRect rtSpaceArea = m_clBGImage.GetSpaceArea(rtClient);
    UpdateArrowInfo(rtSpaceArea);
    m_rtClient = rtSpaceArea;
    m_rtEdge = rtClient;
    CSize clArrowSize = m_aclArrowImage[m_enDirection].GetSize();
    switch (m_enDirection)
    {
    case E_DN_LEFT:
        {
            int iSpace = m_clBGImage.GetLeftSpace();
            m_rtClient.left += clArrowSize.cx - iSpace;
            m_rtEdge.left += clArrowSize.cx - iSpace;
        }
        break;
    case E_DN_TOP:
        {
            int iSpace = m_clBGImage.GetTopSpace();
            m_rtClient.top += clArrowSize.cy - iSpace;
            m_rtEdge.top += clArrowSize.cy - iSpace;
        }
        break;
    case E_DN_RIGHT:
        {
            int iSpace = m_clBGImage.GetRightSpace();
            m_rtClient.right -= clArrowSize.cx - iSpace;
            m_rtEdge.right -= clArrowSize.cx - iSpace;
        }
        break;
    default:
        {
            int iSpace = m_clBGImage.GetBottomSpace();
            m_rtClient.bottom -= clArrowSize.cy - iSpace;
            m_rtEdge.bottom -= clArrowSize.cy - iSpace;
        }
        break;
    }
    m_bUpdateRgn = TRUE;
}

void CI8LabelWnd::SetDesPoint(const CPoint& clPos)
{
    if (m_clDesPos != clPos)
    {
        m_clDesPos = clPos;
        if (GetSafeHwnd() != NULL)
        {
            if (m_bDynamic)
            {
                CRect rtNewWnd = GetDynamicWndRect();
                CRect rtOldWnd;
                GetWindowRect(&rtOldWnd);
                MoveWindow(rtNewWnd);
                if (rtNewWnd.Size() == rtOldWnd.Size())
                {
                    UpdateSize();
                    Invalidate(FALSE);
                }
            }
            else
            {
                UpdateSize();
                Invalidate(FALSE);
            }
        }
    }
}

void CI8LabelWnd::UpdateArrowInfo(const CRect& rtSpaceArea)
{
    CRect rtClient;
    GetClientRect(&rtClient);
    CPoint clLocalPos = m_clDesPos;
    ScreenToClient(&clLocalPos);
    BOOL bHoriDefaultDirection = (m_enDefaultDirection == E_DN_LEFT || m_enDefaultDirection == E_DN_RIGHT);
    BOOL bMustHoriDirection = clLocalPos.y >= rtClient.top && clLocalPos.y < rtClient.bottom;
    if (clLocalPos.x <= rtClient.left && (bHoriDefaultDirection || bMustHoriDirection))
    {
        m_enDirection = E_DN_LEFT;
    }
    else
    {
        if (clLocalPos.x >= rtClient.right - 1 && (bHoriDefaultDirection || bMustHoriDirection))
        {
            m_enDirection = E_DN_RIGHT;
        }
        else
        {
            if (clLocalPos.y <= rtClient.top)
            {
                m_enDirection = E_DN_TOP;
            }
            else
            {
                m_enDirection = E_DN_BOTTOM;
            }
        }
    }
    CSize clArrowSize = m_aclArrowImage[m_enDirection].GetSize();
    if (m_enDirection == E_DN_TOP || m_enDirection == E_DN_BOTTOM)
    {
        m_clArrowPos.x = min(rtSpaceArea.right - 1 - clArrowSize.cx / 2, max(rtSpaceArea.left + clArrowSize.cx / 2, clLocalPos.x));
        if (m_enDirection == E_DN_TOP)
        {
            m_clArrowPos.y = rtClient.top;
        }
        else
        {
            m_clArrowPos.y = rtClient.bottom - 1;
        }
    }
    if (m_enDirection == E_DN_LEFT || m_enDirection == E_DN_RIGHT)
    {
        m_clArrowPos.y = min(rtSpaceArea.bottom - 1 - clArrowSize.cy / 2, max(rtSpaceArea.top + clArrowSize.cy / 2, clLocalPos.y));
        if (m_enDirection == E_DN_LEFT)
        {
            m_clArrowPos.x = rtClient.left;
        }
        else
        {
            m_clArrowPos.x = rtClient.right - 1;
        }
    }
}

void CI8LabelWnd::DrawEgde(HDC hDC)
{
    m_clBGImage.Draw(hDC, m_rtEdge);
    CSize clArrowSize = m_aclArrowImage[m_enDirection].GetSize();
    switch (m_enDirection)
    {
    case E_DN_LEFT:
        m_aclArrowImage[m_enDirection].AlphaBlend(hDC, CPoint(m_clArrowPos.x, m_clArrowPos.y - clArrowSize.cy / 2));
        break;
    case E_DN_TOP:
        m_aclArrowImage[m_enDirection].AlphaBlend(hDC, CPoint(m_clArrowPos.x - clArrowSize.cx / 2, m_clArrowPos.y));
        break;
    case E_DN_RIGHT:
        m_aclArrowImage[m_enDirection].AlphaBlend(hDC, CPoint(m_clArrowPos.x - clArrowSize.cx + 1, m_clArrowPos.y - clArrowSize.cy / 2));
        break;
    default:
        m_aclArrowImage[m_enDirection].AlphaBlend(hDC, CPoint(m_clArrowPos.x - clArrowSize.cx / 2, m_clArrowPos.y - clArrowSize.cy + 1));
        break;
    }
}

BOOL CI8LabelWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    return TRUE;
}

void CI8LabelWnd::Create(CWnd *pPerantWnd, const CRect& rtRect)
{
    CWnd* pWnd = GetForegroundWindow();
    __super::CreateEx(WS_EX_TOPMOST | WS_EX_NOACTIVATE, ms_ptClassName, NULL, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN, rtRect, pPerantWnd, 0);
    if (pWnd != NULL) 
    {
        pWnd->SetForegroundWindow();
    }
}

void CI8LabelWnd::CreateDynamic(CWnd *pPerantWnd, const CSize& clClientSize)
{
    m_bDynamic = TRUE;
    m_rtClient.SetRect(0, 0, clClientSize.cx, clClientSize.cy);
    Create(pPerantWnd, GetDynamicWndRect());
}

void CI8LabelWnd::CreateDynamic(CWnd *pPerantWnd)
{
    m_bDynamic = TRUE;
    CSize clSize = m_clBGImage.GetMidSize();
    m_rtClient.SetRect(0, 0, clSize.cx, clSize.cy);
    Create(pPerantWnd, GetDynamicWndRect());
}

CRect CI8LabelWnd::GetDynamicWndRect() const
{
    CRect rtWnd(0, 0, m_rtClient.Width() + m_clBGImage.GetLeftSpace() + m_clBGImage.GetRightSpace(),
        m_rtClient.Height() + m_clBGImage.GetTopSpace() + m_clBGImage.GetBottomSpace());
    if (m_enDefaultDirection == E_DN_LEFT || m_enDefaultDirection == E_DN_RIGHT)
    {
        rtWnd.MoveToY(max(m_clDesPos.y - rtWnd.Height() / 2, 0));
    }
    else
    {
        rtWnd.MoveToX(max(m_clDesPos.x - rtWnd.Width() / 2, 0));
    }
    int iScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
    int iArrowLength;
    switch (m_enDefaultDirection)
    {
    case E_DN_LEFT:
        iArrowLength = m_aclArrowImage[m_enDefaultDirection].GetSize().cx - m_clBGImage.GetLeftSpace();
        if (m_clDesPos.x + m_iDesPointInterval + iArrowLength + rtWnd.Width() <= iScreenWidth)
        {
            rtWnd.right += iArrowLength;
            rtWnd.MoveToX(m_clDesPos.x + m_iDesPointInterval);
        }
        else
        {
            iArrowLength = m_aclArrowImage[E_DN_RIGHT].GetSize().cx - m_clBGImage.GetRightSpace();
            rtWnd.right += iArrowLength;
            rtWnd.MoveToX(m_clDesPos.x - m_iDesPointInterval - rtWnd.Width());
        }
        break;
    case E_DN_TOP:
        iArrowLength = m_aclArrowImage[m_enDefaultDirection].GetSize().cy - m_clBGImage.GetTopSpace();
        if (m_clDesPos.y + m_iDesPointInterval + iArrowLength + rtWnd.Height() <= iScreenHeight)
        {
            rtWnd.bottom += iArrowLength;
            rtWnd.MoveToY(m_clDesPos.y + m_iDesPointInterval);
        }
        else
        {
            iArrowLength = m_aclArrowImage[E_DN_BOTTOM].GetSize().cy - m_clBGImage.GetBottomSpace();
            rtWnd.bottom += iArrowLength;
            rtWnd.MoveToY(m_clDesPos.y - m_iDesPointInterval - rtWnd.Height());
        }
        break;
    case E_DN_RIGHT:
        iArrowLength = m_aclArrowImage[m_enDefaultDirection].GetSize().cx - m_clBGImage.GetRightSpace();
        if (m_clDesPos.x - m_iDesPointInterval - iArrowLength - rtWnd.Width() >= 0)
        {
            rtWnd.right += iArrowLength;
            rtWnd.MoveToX(m_clDesPos.x - m_iDesPointInterval - rtWnd.Width());
        }
        else
        {
            iArrowLength = m_aclArrowImage[E_DN_LEFT].GetSize().cx - m_clBGImage.GetLeftSpace();
            rtWnd.right += iArrowLength;
            rtWnd.MoveToX(m_clDesPos.x + m_iDesPointInterval);
        }
        break;
    default:
        iArrowLength = m_aclArrowImage[m_enDefaultDirection].GetSize().cy - m_clBGImage.GetBottomSpace();
        if (m_clDesPos.y - m_iDesPointInterval - iArrowLength - rtWnd.Height() >= 0)
        {
            rtWnd.bottom += iArrowLength;
            rtWnd.MoveToY(m_clDesPos.y - m_iDesPointInterval - rtWnd.Height());
        }
        else
        {
            iArrowLength = m_aclArrowImage[E_DN_TOP].GetSize().cy - m_clBGImage.GetTopSpace();
            rtWnd.bottom += iArrowLength;
            rtWnd.MoveToY(m_clDesPos.y + m_iDesPointInterval);
        }
        break;
    }
    return rtWnd;
}

void CI8LabelWnd::SetDefaultDirection(Direction_en enDefaultDirection)
{
    if (enDefaultDirection != m_enDefaultDirection)
    {
        m_enDefaultDirection = enDefaultDirection;
        if (GetSafeHwnd() != NULL && m_bDynamic)
        {
            MoveWindow(GetDynamicWndRect());
        }
    }
}
void CI8LabelWnd::SetDesPointInterval(int iDesPointInterval)
{
    assert(iDesPointInterval >= 0);
    if (m_iDesPointInterval != iDesPointInterval)
    {
        m_iDesPointInterval = iDesPointInterval;
        if (GetSafeHwnd() != NULL && m_bDynamic)
        {
            MoveWindow(GetDynamicWndRect());
        }
    }
}

void CI8LabelWnd::OnDraw(HDC hDC, const CRect& rtClient)
{
}

