// LabelWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "I8BaseTooltip.h"

using namespace I8SkinCtrl_ns;

const int C_TIMER_EVENT_WAIT_START = 21217; //等待提示框事件
const int C_TIMER_EVENT_CLOSE = 21247; //提示框窗口自动关闭事件
const int C_TIMER_EVENT_CHECK = 21257; //检查鼠标位置事件
const int C_CHECK_TIME_INTERVAL = 100; //检查鼠标位置时间间隔

IMPLEMENT_DYNAMIC(CI8BaseTooltip, CI8LabelWnd)

BEGIN_MESSAGE_MAP(CI8BaseTooltip, CI8LabelWnd)
    ON_WM_TIMER()
END_MESSAGE_MAP()

CI8BaseTooltip::CI8BaseTooltip()
    : m_bEnableTooltip(FALSE)
    , m_iStartDelayTime(1000)
    , m_iDelayTime(0)
    , m_pCurArea(NULL)
    , m_enState(E_ST_CLOSE)
    , m_hMSGWnd(NULL)
    , m_iAlpha(255)
    , m_iDisappearTime(4000)
{
    m_clLastCursorPos.SetPoint(0, 0);
}

CI8BaseTooltip::~CI8BaseTooltip()
{
}

void CI8BaseTooltip::EnableTooltip(BOOL bEnable)
{
    m_bEnableTooltip = bEnable;
}

void CI8BaseTooltip::SetStartDelayTime(int iStartDelayTime)
{
    assert(iStartDelayTime > 0);
    m_iStartDelayTime = iStartDelayTime;
}

void CI8BaseTooltip::SetDelayTime(int iDelayTime)
{
    assert(iDelayTime > 0);
    m_iDelayTime = iDelayTime;
}

void CI8BaseTooltip::AddArea(const CRect& rtRect, UINT uiInfo)
{
    m_clArea.push_back(std::make_pair(rtRect, uiInfo));
}

void I8SkinCtrl_ns::CI8BaseTooltip::SetWnd(HWND hWnd)
{
    assert(hWnd != NULL && m_hMSGWnd == NULL);
    m_hMSGWnd = hWnd;
}
void CI8BaseTooltip::ClearArea()
{
    m_clArea.clear();
}

void CI8BaseTooltip::RelayEvent(const MSG* pMSG)
{
    assert(pMSG != NULL);
    if (m_bEnableTooltip && pMSG->hwnd == m_hMSGWnd)
    {
        switch (pMSG->message)
        {
        case WM_MOUSEMOVE:
            switch (m_enState)
            {
            case E_ST_WAIT_START:
                ::KillTimer(pMSG->hwnd, C_TIMER_EVENT_WAIT_START);
                m_enState = E_ST_CLOSE;
            case E_ST_CLOSE:
                if (m_clArea.size() == 0 || IsMouseInArea(m_hMSGWnd))
                {
                    ::SetTimer(pMSG->hwnd, C_TIMER_EVENT_WAIT_START, m_iStartDelayTime, WaitTimeProc);
                    GetCursorPos(&m_clLastCursorPos);
                    m_enState = E_ST_WAIT_START;
                }
                break;
            default:
                break;
            }
            break;
        case WM_MOVE:
        case WM_SIZE:
            if (m_enState != E_ST_CLOSE)
            {
                CloseTooltipWnd();
            }
            break;
        case WM_TIMER:
            if (pMSG->wParam == C_TIMER_EVENT_WAIT_START)
            {
                ::KillTimer(pMSG->hwnd, C_TIMER_EVENT_WAIT_START);
                 CreateTooltipWnd();
            }
            break;
        default:
            break;
        }
    }
}

void I8SkinCtrl_ns::CI8BaseTooltip::CreateTooltipWnd()
{
    assert(m_hMSGWnd != NULL);
    assert(m_enState == E_ST_WAIT_START);
    CPoint clPos;
    GetCursorPos(&clPos);
    if (clPos != m_clLastCursorPos)
    {
        m_enState = E_ST_CLOSE;
    }
    else
    {
        m_enState = E_ST_DISPLAY;
        UpdateCurArea();
        SetDesPoint(GetDesPoint());
        CSize clClientSize(0,  0);
        CalculateClientSize(clClientSize);
        if (clClientSize.cx == 0 || clClientSize.cy == 0)
        {
            CreateDynamic(CWnd::FromHandle(m_hMSGWnd));
        }
        else
        {
            CreateDynamic(CWnd::FromHandle(m_hMSGWnd), clClientSize);
        }
        ModifyStyleEx(0, WS_EX_LAYERED);
        SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
        SetTimer(C_TIMER_EVENT_CHECK, C_CHECK_TIME_INTERVAL, NULL);
    }
}

void I8SkinCtrl_ns::CI8BaseTooltip::CloseTooltipWnd()
{
    if (m_enState != E_ST_CLOSE)
    {
        assert(m_hMSGWnd != NULL);
        switch (m_enState)
        {
        case E_ST_WAIT_START:
            ::KillTimer(m_hMSGWnd, C_TIMER_EVENT_WAIT_START);
            break;
        case E_ST_DISPLAY:
            KillTimer(C_TIMER_EVENT_CHECK);
            DestroyWindow();
            break;
        case E_ST_WAIT_CLOSE:
            KillTimer(C_TIMER_EVENT_CHECK);
            KillTimer(C_TIMER_EVENT_CLOSE);
            DestroyWindow();
            break;
        case E_ST_CLOSING:
            KillTimer(C_TIMER_EVENT_CHECK);
            DestroyWindow();
            break;
        default:
            break;
        }
        m_enState = E_ST_CLOSE;
        m_pCurArea = NULL;
    }
}

CPoint CI8BaseTooltip::GetDesPoint() const
{
    assert(m_hMSGWnd != NULL);
    if (m_clArea.size() == 0)
    {
        CRect rtWnd;
        ::GetWindowRect(m_hMSGWnd, &rtWnd);
        return CPoint(rtWnd.left + rtWnd.Width() / 2, rtWnd.top + rtWnd.Height() / 2);
    }
    else
    {
        assert(m_pCurArea != NULL);
        CRect rtArea = m_pCurArea->first;
        CPoint clDesPos(m_pCurArea->first.left + m_pCurArea->first.Width() / 2, m_pCurArea->first.top + m_pCurArea->first.Height() / 2);
        ::ClientToScreen(m_hMSGWnd, &clDesPos);
        return clDesPos;
    }
}
void CI8BaseTooltip::OnTimer(UINT_PTR nIDEvent)
{
    __super::OnTimer(nIDEvent);
    switch (nIDEvent)
    {
    case C_TIMER_EVENT_CHECK:
        switch (m_enState)
        {
        case E_ST_DISPLAY:
            if (!IsMouseInTooltip() && !IsMouseInCurArea())
            {
                if (m_iDelayTime == 0)
                {
                    m_enState = E_ST_CLOSING;
                    m_iAlpha = 255;
                }
                else
                {
                    m_enState = E_ST_WAIT_CLOSE;
                    SetTimer(C_TIMER_EVENT_CLOSE, m_iDelayTime, NULL);
                }
            }
            break;
        case E_ST_WAIT_CLOSE:
            if (IsMouseInTooltip())
            {
                m_enState = E_ST_DISPLAY;
                KillTimer(C_TIMER_EVENT_CLOSE);
            }
            break;
        case E_ST_CLOSING:
            if (!IsMouseInTooltip())
            {
                m_iAlpha -= m_iDisappearTime / C_CHECK_TIME_INTERVAL;
                if (m_iAlpha <= 0)
                {
                    CloseTooltipWnd();
                }
                else
                {
                    SetLayeredWindowAttributes(0, m_iAlpha, LWA_ALPHA);
                }
            }
            else
            {
                m_enState = E_ST_DISPLAY;
                SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
            }
            break;
        default:
            break;
        }
        break;
    case C_TIMER_EVENT_CLOSE:
        assert(m_enState == E_ST_WAIT_CLOSE);
        if (!IsMouseInCurArea())
        {
            m_iAlpha = 255;
            m_enState = E_ST_CLOSING;
            KillTimer(C_TIMER_EVENT_CLOSE);
            KillTimer(C_TIMER_EVENT_CHECK);
            SetTimer(C_TIMER_EVENT_CHECK, C_CHECK_TIME_INTERVAL, NULL);
        }
        else
        {
            m_enState = E_ST_DISPLAY;
            KillTimer(C_TIMER_EVENT_CLOSE);
        }
        break;
    default:
        break;
    }
}

int CI8BaseTooltip::GetCurAreaIndex() const
{
    assert(m_pCurArea != NULL);
    int iIndex = 0;
    for (std::list<std::pair<CRect, UINT>>::const_iterator clId = m_clArea.begin(); clId != m_clArea.end(); ++clId)
    {
        if (&(*clId) == m_pCurArea)
        {
            return iIndex;
        }
        ++iIndex;
    }
    assert(0);
    return -1;
}

UINT CI8BaseTooltip::GetCurAreaInfo() const
{
    assert(m_pCurArea != NULL);
    return m_pCurArea->second;
}

void CI8BaseTooltip::CalculateClientSize(CSize& clClientSize)
{
}

void CALLBACK EXPORT CI8BaseTooltip::WaitTimeProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
}

BOOL CI8BaseTooltip::IsMouseInTooltip() const
{
    CPoint clPos;
    GetCursorPos(&clPos);
    ScreenToClient(&clPos);
    CRect rtClient;
    GetClientRect(&rtClient);
    return rtClient.PtInRect(clPos);
}

BOOL CI8BaseTooltip::IsMouseInArea(HWND hWnd) const
{
    CPoint clPos;
    GetCursorPos(&clPos);
    ::ScreenToClient(hWnd, &clPos);
    if (m_clArea.size() == 0)
    {
        CRect rtClient;
        ::GetClientRect(hWnd, &rtClient);
        return rtClient.PtInRect(clPos);
    }
    else
    {
        for (std::list<std::pair<CRect, UINT>>::const_iterator clId = m_clArea.begin(); clId != m_clArea.end(); ++clId)
        {
            if (clId->first.PtInRect(clPos))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL CI8BaseTooltip::IsMouseInCurArea() const
{
    CPoint clPos;
    GetCursorPos(&clPos);
    ::ScreenToClient(m_hMSGWnd, &clPos);
    if (m_clArea.size() == 0)
    {
        CRect rtClient;
        ::GetClientRect(m_hMSGWnd, &rtClient);
        return rtClient.PtInRect(clPos);
    }
    else
    {
        assert(m_pCurArea != NULL);
        for (std::list<std::pair<CRect, UINT>>::const_iterator clId = m_clArea.begin(); clId != m_clArea.end(); ++clId)
        {
            if (clId->first.PtInRect(clPos))
            {
                return &(*clId) == m_pCurArea;
            }
        }
    }
    return FALSE;
}

void CI8BaseTooltip::SetDisappearTime(int iDisappearTime)
{
    assert(iDisappearTime >= 0);
    m_iDisappearTime = iDisappearTime;
}
void CI8BaseTooltip::UpdateCurArea()
{
    m_pCurArea = NULL;
    CPoint clPos;
    GetCursorPos(&clPos);
    ::ScreenToClient(m_hMSGWnd, &clPos);
    if (m_clArea.size() != 0)
    {
        for (std::list<std::pair<CRect, UINT>>::const_iterator clId = m_clArea.begin(); clId != m_clArea.end(); ++clId)
        {
            if (clId->first.PtInRect(clPos))
            {
                m_pCurArea = &(*clId);
                break;
            }
        }
        assert(m_pCurArea != NULL);
    }
}
