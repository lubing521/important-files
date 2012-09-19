#include "stdafx.h"
#include "I8ButtonBar.h"
#include "I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

const CSize CI8ButtonBar::C_BUTTON_SIZE_IMAGE = CSize(-1, -1);
const CSize CI8ButtonBar::C_BUTTON_SIZE_NULL = CSize(0, 0);

IMPLEMENT_DYNAMIC(CI8ButtonBar, CWnd)

BEGIN_MESSAGE_MAP(CI8ButtonBar, CWnd)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_COMMAND_RANGE(0, 0x0fffffff, OnButtonClick)
END_MESSAGE_MAP()

CI8ButtonBar::CI8ButtonBar()
    : m_iButtonHoriSpace(0)
    , m_iLeftSpace(0)
    , m_iRightSpace(0)
    , m_iTopSpace(-1)
{
}

CI8ButtonBar::~CI8ButtonBar()
{
}

void CI8ButtonBar::Create(DWORD dwStyle, const CRect& rtRect, CWnd* pclWnd, const CSize& szSize, const ButtonInfo_st* pstButtonInfo, int iCount, const stdex::tString& sBaseDir)
{
    assert(GetSafeHwnd() == NULL);
    assert(pclWnd != NULL);
    assert(szSize.cx >= 0 && szSize.cy >= 0);
    assert(pstButtonInfo != NULL);
    assert(iCount >= 0);
    if (!__super::Create(NULL, NULL, dwStyle, rtRect, pclWnd, 0))
    {
        assert(0 && "创建按钮条控件失败");
    }
    m_clButton.ApplyArray(iCount);
    for (int i = 0; i < m_clButton.GetCount(); ++i)
    {
        m_clButton[i].Load(sBaseDir + pstButtonInfo[i].sSkinName);
        CSize clButtonSize(szSize);
        if (pstButtonInfo[i].szButtonSize == C_BUTTON_SIZE_IMAGE)
        {
            clButtonSize = m_clButton[i].GetImageSize();
        }
        else
        {
            if (pstButtonInfo[i].szButtonSize == C_BUTTON_SIZE_NULL)
            {
                clButtonSize = pstButtonInfo[i].szButtonSize;
            }
        }
        m_clButton[i].Create(pstButtonInfo[i].sText.c_str(), WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | BS_PUSHBUTTON, CRect(0, 0, clButtonSize.cx, clButtonSize.cy), this, pstButtonInfo[i].nID);
    }
    AdjustSize();
}

void CI8ButtonBar::SetHoriSpace(int iButtonHoriSpace)
{
    m_iButtonHoriSpace = iButtonHoriSpace;
    if (GetSafeHwnd() != NULL)
    {
        AdjustSize();
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}

void CI8ButtonBar::SetHoriEdge(int iLeftSpace, int iRightSpace)
{
    m_iLeftSpace = iLeftSpace;
    m_iRightSpace = iRightSpace;
    if (GetSafeHwnd() != NULL)
    {
        AdjustSize();
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}

void CI8ButtonBar::SetTopEdge(int iTopSpace)
{
    assert(iTopSpace != -1 && "按钮条的顶边距不能为-1，-1被用于默认垂直居中");
    m_iTopSpace = iTopSpace;
    if (GetSafeHwnd() != NULL)
    {
        AdjustSize();
        m_clBGCache.NeedRedraw();
        Invalidate();
    }
}

CI8Button& CI8ButtonBar::GetButton(UINT nID)
{
    assert(GetSafeHwnd() != NULL && "按钮条还没有被创建");
    CI8Button* pclButton = dynamic_cast<CI8Button*>(GetDlgItem(nID));
    assert(pclButton != NULL && "根据ID在按钮条上找不到对应按钮");
    return *pclButton;
}

void CI8ButtonBar::SetBackground(const stdex::tString& sSkinName)
{
    m_clBGImage.Load(sSkinName);
    m_clBGCache.NeedRedraw();
    if (GetSafeHwnd() != NULL)
    {
        Invalidate();
    }
}

void CI8ButtonBar::AdjustSize()
{
    CRect rtClient;
    GetClientRect(&rtClient);
	CRect rtValidRect = rtClient;
    rtValidRect.left += m_iLeftSpace;
    rtValidRect.right -= m_iRightSpace;
    if (m_iTopSpace != -1)
    {
        rtValidRect.top += m_iTopSpace;
    }
    for (int i = 0; i < m_clButton.GetCount(); ++i)
    {
        CRect rtButtonRect;
        m_clButton[i].GetClientRect(&rtButtonRect);
        rtButtonRect.MoveToXY(rtValidRect.left, m_iTopSpace == -1 ? (rtValidRect.top + (rtValidRect.Height() - rtButtonRect.Height()) / 2) : rtValidRect.top);
        m_clButton[i].MoveWindow(rtButtonRect);
        rtValidRect.left += rtButtonRect.Width();
    }
}

void CI8ButtonBar::OnSize(IN UINT nType, IN int cx, IN int cy)
{
    if (m_clParentBGBrush.GetSafeHandle() != NULL)
    {
        m_clParentBGBrush.DeleteObject();
    }
    m_clBGCache.SetSize(CSize(cx, cy));
    AdjustSize();
    return __super::OnSize(nType, cx, cy);
}

void CI8ButtonBar::OnMove(int x, int y)
{
    if (m_clParentBGBrush.GetSafeHandle() != NULL)
    {
        m_clParentBGBrush.DeleteObject();
    }
    return __super::OnMove(x, y);
}

BOOL CI8ButtonBar::OnEraseBkgnd(CDC* pDC)
{
    if (m_clBGCache.IsNeedRedraw())
    {
        CI8Bitmap& clImage = m_clBGCache.GetImage();
        CI8Bitmap::CI8Draw clDraw;
        HDC hDC = clImage.GetDC(clDraw, *pDC);
        I8_FillParentWndBG(*this, hDC, m_clParentBGBrush);
        if (!m_clBGImage.IsNull())
        {
            CRect rtClient;
            GetClientRect(&rtClient);
            m_clBGImage.Draw(hDC, rtClient);
        }
    }
    m_clBGCache.Draw(*pDC);
    return TRUE;
}

void CI8ButtonBar::OnPaint()
{
    CPaintDC dc(this);
}

void CI8ButtonBar::OnButtonClick(UINT nID)
{
    GetParent()->PostMessage(WM_COMMAND, MAKELONG(nID, BN_CLICKED), reinterpret_cast<LPARAM>(GetDlgItem(nID)->GetSafeHwnd()));
}

CSize CI8ButtonBar::GetSize(const CSize& szSize, const ButtonInfo_st* pstButtonInfo, int iCount)
{
    assert(szSize.cx > 0 && szSize.cy > 0);
    assert(pstButtonInfo != NULL);
    assert(iCount >= 0);
    int iTopSpace = (m_iTopSpace != -1 ? m_iTopSpace : 0);
    CSize clOutSize(m_iLeftSpace + m_iRightSpace, m_clBGImage.IsNull() ? iTopSpace : m_clBGImage.GetSize().cy);
    for (int i = 0; i < m_clButton.GetCount(); ++i)
    {
        if (pstButtonInfo[i].szButtonSize.cx !=  0)
        {
            clOutSize.cx += pstButtonInfo[i].szButtonSize.cx;
        }
        else
        {
            clOutSize.cx += szSize.cx;
        }
        if (m_clBGImage.IsNull())
        {
            if (pstButtonInfo[i].szButtonSize.cx !=  0)
            {
                clOutSize.cy = max(clOutSize.cy, pstButtonInfo[i].szButtonSize.cy + iTopSpace);
            }
            else
            {
                clOutSize.cy = max(clOutSize.cy, szSize.cy + iTopSpace);
            }
        }
    }
    return clOutSize;
}