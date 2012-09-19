#include "stdafx.h"
#include "I8Button.h"
#include "I8SkinPack.h"
#include "I8SkinCommonFunction.h"
#include <windowsx.h>

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8Button, CButton)

BEGIN_MESSAGE_MAP(CI8Button, CButton)
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_CREATE()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_MESSAGE(BM_SETSTATE, OnSetState)
END_MESSAGE_MAP()

CI8Button::CI8Button()
    : m_bHover(FALSE)
    , m_bCreate(FALSE)
{
}

CI8Button::~CI8Button()
{
}

void CI8Button::Load(const stdex::tString& sName)
{
    CI8SkinPack::Instance().LoadSkin(m_aclBGImage[E_BS_NORMAL], sName + _T("_普通.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclBGImage[E_BS_HOVER], sName + _T("_鼠标经过.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclBGImage[E_BS_PRESS], sName + _T("_鼠标按下.png"));
    CI8SkinPack::Instance().LoadSkin(m_aclBGImage[E_BS_DISABLE], sName + _T("_不可用.png"), FALSE);
    if (m_aclBGImage[E_BS_DISABLE].IsNull())
    {
        m_aclBGImage[E_BS_DISABLE] = m_aclBGImage[E_BS_NORMAL];
    }
    m_clBGCache.NeedRedraw();

    if (IsWindow(GetSafeHwnd()))
    {
        Invalidate();
    }
}


void CI8Button::Init()
{
    if (m_aclBGImage[E_BS_NORMAL].IsNull())
    {
        Load(CI8SkinPack::Instance().GetDefaultSkin(WC_BUTTON));
    }

    ModifyStyle(0, BS_OWNERDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	m_uiTextColor[E_BS_NORMAL] = RGB(0, 0, 0);
	m_uiTextColor[E_BS_HOVER] = RGB(0, 0, 0);
	m_uiTextColor[E_BS_PRESS] = RGB(0, 0, 0);
	m_uiTextColor[E_BS_DISABLE] = RGB(128, 128, 128);
	
	m_uiTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;

  
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	if( m_clFont.m_hObject != 0 )
		m_clFont.DeleteObject();

	m_clFont.CreateFontIndirect(&lf);
}


void CI8Button::PreSubclassWindow()
{
    __super::PreSubclassWindow();
    if (!m_bCreate)
    {
        Init();
        //绑定的控件时没有收到窗口大小改变消息，只能手动设置背景缓存大小
        CRect rtClient;
        GetClientRect(&rtClient);
        m_clBGCache.SetSize(rtClient.Size());
    }
}


void CI8Button::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	LPNMCUSTOMDRAW *custom = reinterpret_cast<LPNMCUSTOMDRAW *>(lpDis);

    CRect rcClient(lpDis->rcItem);

    if (m_clBGCache.IsNeedRedraw())
    {
        CI8Bitmap& clImage = m_clBGCache.GetImage();
        CI8Bitmap::CI8Draw clDraw;
        HDC hDC = clImage.GetDC(clDraw, lpDis->hDC);
        I8_FillParentWndBG(*this, hDC, m_clParentBGBrush);

        ButtonState_en enState = GetState();
        m_aclBGImage[enState].AlphaBlend(hDC, rcClient);

        // Text
        CString sText;
        GetWindowText(sText);
        HFONT hOldFont = SelectFont(hDC, m_clFont);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, m_uiTextColor[enState]);
        DrawText(hDC, sText, sText.GetLength(), &rcClient, m_uiTextStyle);
        SelectFont(hDC, hOldFont);
    }

    m_clBGCache.Draw(lpDis->hDC);
}

int CI8Button::OnCreate(LPCREATESTRUCT lp)
{
    if (m_bCreate)
    {
	    Init();
    }
	return __super::OnCreate(lp);
}

BOOL CI8Button::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CI8Button::OnSize(UINT flag, int x, int y)
{
    if (m_clParentBGBrush.GetSafeHandle() != NULL)
    {
        m_clParentBGBrush.DeleteObject();
    }

    m_clBGCache.SetSize(CSize(x, y));

	return __super::OnSize(flag, x, y);
}

void CI8Button::OnMove(int x, int y)
{
    if (m_clParentBGBrush.GetSafeHandle() != NULL)
    {
        m_clParentBGBrush.DeleteObject();
    }

	return __super::OnMove(x, y);
}


void CI8Button::OnMouseMove(UINT, CPoint)
{
	if( !m_bHover ) 
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		_TrackMouseEvent(&tme);

		m_bHover = TRUE;
        m_clBGCache.NeedRedraw();
		Invalidate();
	}

}

void CI8Button::OnMouseLeave()
{
	if( m_bHover ) 
	{
        m_bHover = FALSE;
        m_clBGCache.NeedRedraw();
		Invalidate();
	}
}

CI8Button::ButtonState_en CI8Button::GetState() const
{
    UINT uiState = __super::GetState();
    if (IsWindowEnabled())
    {
        if (uiState & BST_PUSHED)
        {
            return E_BS_PRESS;
        }
        else
        {
            if (m_bHover)
            {
                return E_BS_HOVER;
            }
            else
            {
                return E_BS_NORMAL;
            }
        }
    }
    else
    {
        return E_BS_DISABLE;
    }
}

BOOL CI8Button::PreCreateWindow(CREATESTRUCT& cs)
{
    m_bCreate = TRUE;
    return __super::PreCreateWindow(cs);
}


LRESULT CI8Button::OnSetState(WPARAM wParam, LPARAM lParam)
{
    LRESULT iResult;
    if (IsWindowVisible())
    {
        SetRedraw(FALSE);
        iResult = Default();
        SetRedraw(TRUE);
    }
    else
    {
        iResult = Default();
    }
    m_clBGCache.NeedRedraw();
    Invalidate();
    return iResult;
}

CSize CI8Button::GetImageSize() const
{
    if (m_aclBGImage[E_BS_NORMAL].IsNull())
    {
        assert(0 && "图像没有加载");
    }
    return m_aclBGImage[E_BS_NORMAL].GetSize();
}
