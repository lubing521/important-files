// ButtonEx.cpp : 实现文件
//
#include "stdafx.h"
#include "ui.h"

#pragma warning(disable:4244)

using namespace std;
using namespace Gdiplus;

#define DOCK_NAVBAR_INDEX			0x00002000
#define DOCK_STABAR_INDEX			0x00002100
#define DOCK_BROWSE_INDEX			0x00002200
#define DOCK_FINDER_INDEX			0x00002300
#define DOCK_TAB_INDEX				0x00002400

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CButtonEx, CButton)

CButtonEx::CButtonEx()
{
	m_Status = BEX_NORMAL;
	m_bgBack = m_bgHover = m_bgPressed = m_bgActive = RGB(0, 0, 0);
	m_pbkImage = m_pbkImage2 = m_pbkImage3 = NULL;
	m_phoverImage = m_phoverImage2 = m_phoverImage3 = NULL;
	m_pselectImage = m_pselectImage2 = m_pselectImage3 = NULL;
	m_pActiveImage = m_piconImage = m_piconSelImage = NULL;
	m_pdisableImage = NULL;

	m_szFont = DEFAULT_FONT_NAME;
	m_nFontSize = 12;
	m_clrText = RGB(0, 0, 0);
	m_clrHover = RGB(0, 0, 0);
	m_clrSel   = RGB(0, 0, 0);
	m_clrActive = RGB(0, 0, 0);
	m_nBTStyle = BTEXT_LEFT;
	m_bBold = FALSE;
	m_bActive = FALSE;
	m_hCursor = NULL;
}

CButtonEx::~CButtonEx()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pbkImage2)
	{
		m_pbkImage2->DeleteObject();
		delete m_pbkImage2;
	}
	if (m_pbkImage3)
	{
		m_pbkImage3->DeleteObject();	
		delete m_pbkImage3;
	}

	if (m_phoverImage)
	{
		m_phoverImage->DeleteObject();
		delete m_phoverImage;
	}
	if (m_phoverImage2)
	{
		m_phoverImage2->DeleteObject();
		delete m_phoverImage2;
	}
	if (m_phoverImage3)
	{
		m_phoverImage3->DeleteObject();
		delete m_phoverImage3;
	}

	if (m_pselectImage)
	{
		m_pselectImage->DeleteObject();
		delete m_pselectImage;
	}
	if (m_pselectImage2)
	{
		m_pselectImage2->DeleteObject();
		delete m_pselectImage2;
	}
	if (m_pselectImage3)
	{
		m_pselectImage3->DeleteObject();
		delete m_pselectImage3;
	}

	if (m_pActiveImage)
	{
		m_pActiveImage->DeleteObject();
		delete m_pActiveImage;
	}
	if (m_piconImage)
	{
		m_piconImage->DeleteObject();
		delete m_piconImage;
	}
	if (m_piconSelImage)
	{
		m_piconSelImage->DeleteObject();
		delete m_piconSelImage;
	}

	if (m_pdisableImage)
	{
		m_pdisableImage->DeleteObject();
		delete m_pdisableImage;
	}

	if (m_hCursor != NULL)
		DestroyCursor(m_hCursor);
}

BEGIN_MESSAGE_MAP(CButtonEx, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void  CButtonEx::SetImage(DWORD backImage, DWORD hoverImage, DWORD selectImage, DWORD activeImage)
{
	m_pbkImage2		= AfxGetSkinImage(backImage);
	m_phoverImage2	= AfxGetSkinImage(hoverImage);
	m_pselectImage2	= AfxGetSkinImage(selectImage);
	m_pActiveImage = AfxGetSkinImage(activeImage);
}

void CButtonEx::SetImage(LPCTSTR backImage /* = NULL */, LPCTSTR hoverImage /* = NULL */, 
						 LPCTSTR selectImage /* = NULL */, LPCTSTR activeImage /* = NULL */,
						 LPCTSTR disableImage /*= NULL*/)
{
	m_pbkImage2		= AfxGetSkinImage(backImage);
	m_phoverImage2	= AfxGetSkinImage(hoverImage);
	m_pselectImage2	= AfxGetSkinImage(selectImage);
	m_pActiveImage  = AfxGetSkinImage(activeImage);
	m_pdisableImage = AfxGetSkinImage(disableImage);
}

void CButtonEx::SetImage2(LPCTSTR backImage, LPCTSTR backImage2, LPCTSTR backImage3, 
						  LPCTSTR selectImage, LPCTSTR selectImage2, LPCTSTR selectImage3)
{
	m_pbkImage		= AfxGetSkinImage(backImage);
	m_pbkImage2		= AfxGetSkinImage(backImage2);
	m_pbkImage3		= AfxGetSkinImage(backImage3);
	m_pselectImage	= AfxGetSkinImage(selectImage);
	m_pselectImage2	= AfxGetSkinImage(selectImage2);
	m_pselectImage3	= AfxGetSkinImage(selectImage3);
}

void  CButtonEx::SetImage3(COLORREF backColor, COLORREF hoverColor, COLORREF selectColor, COLORREF activeColor)
{
	m_bgBack	= backColor;
	m_bgHover	= hoverColor;
	m_bgPressed	= selectColor;
	m_bgActive	= activeColor;
}

void CButtonEx::SetIconImage(LPCTSTR iconImage, LPCTSTR iconSelImage)
{
	m_piconImage	= AfxGetSkinImage(iconImage);
	m_piconSelImage = AfxGetSkinImage(iconSelImage);
}

void CButtonEx::SetIconImage(HICON hIcon, HICON hSelIcon)
{
	if (m_piconImage != NULL)
	{
		delete m_piconImage;
		m_piconImage = NULL;
	}
	if (m_piconSelImage != NULL)
	{
		delete m_piconSelImage;
		m_piconSelImage = NULL;
	}

	m_piconImage = AfxGetSkinImage(hIcon);
	m_piconSelImage = AfxGetSkinImage(hSelIcon);

	if (m_hWnd != NULL)
	{
		Invalidate();
	}
}

void  CButtonEx::SetTextStyle(CString font, int nSize, COLORREF clrText, COLORREF clrHover, 
	COLORREF clrSel, COLORREF clrActive, BTEXT_STYLE nStyle, BOOL bBold)
{
	m_szFont = font;
	m_nFontSize = nSize;
	m_clrText = clrText;
	m_clrHover = clrHover;
	m_nBTStyle = nStyle;
	m_bBold = bBold;
	m_clrSel = clrSel;
	m_clrActive = clrActive;
	if (m_hWnd != NULL)
		Invalidate();
}

void CButtonEx::SetActive(BOOL bActive)
{
	BOOL bOld = m_bActive;
	m_bActive = bActive; 
	if (bOld != m_bActive)
		Invalidate();
}

void CButtonEx::PreSubclassWindow()
{
 	ModifyStyle(0, BS_OWNERDRAW);
	long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	CButton::PreSubclassWindow();
}

void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rcClient(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	pDC->SetBkMode(1);

	//选择图片
	COLORREF bgColor = RGB(0, 0, 0);
	CBitmap *pBitmap = NULL, *pBitmap2 = NULL, *pBitmap3 = NULL;
	if (!IsWindowEnabled())		//先判断是否是禁用。
	{
		pBitmap2 = m_pdisableImage;
		pBitmap = pBitmap3 = NULL;
	}
	else if (m_bActive)	//再判断是否是激活。
	{
		bgColor = m_bgActive;
		pBitmap2 = m_pActiveImage;
		if (pBitmap2 == NULL)
		{
			pBitmap = m_pselectImage;
			pBitmap2 = m_pselectImage2;
			pBitmap3 = m_pselectImage3;
		}
	}
	else
	{
		if (m_Status == BEX_PRESSED)	//按钮按下的状态。
		{
			pBitmap = m_pselectImage;
			pBitmap2 = m_pselectImage2;
			pBitmap3 = m_pselectImage3;
			bgColor = m_bgPressed;
		}
		else if (m_Status == BEX_HOVER)	//鼠标放在按钮上的状态。
		{
			if (m_phoverImage2 != NULL)
			{
				pBitmap = m_phoverImage;
				pBitmap2 = m_phoverImage2;
				pBitmap3 = m_phoverImage3;
			}
			bgColor = m_bgHover;
		}
		else							//正常
		{
			bgColor = m_bgBack;
			pBitmap = m_pbkImage;
			pBitmap2 = m_pbkImage2;
			pBitmap3 = m_pbkImage3;
		}
	}

	if (pBitmap2 == NULL)
	{
		pBitmap = m_pbkImage;
		pBitmap2 = m_pbkImage2;
		pBitmap3 = m_pbkImage3;
	}

	//绘背景图片
	if (pBitmap2 != NULL)
	{
		
		if (pBitmap == NULL || pBitmap3 == NULL)	//只有一张图片的情况(使用第２张图片来画)
		{
			AfxDrawImage(pDC, pBitmap2, rcClient);
		}
		else	//有三张图片的情况。第１，第３不需要拉申，第２张可能需要拉伸
		{
			BITMAP bmp1;
			pBitmap->GetBitmap(&bmp1);
			AfxDrawImage(pDC, pBitmap, CRect(0, 0, bmp1.bmWidth, bmp1.bmHeight));
			
			BITMAP bmp3;
			pBitmap3->GetBitmap(&bmp3);
			CRect rc = CRect(rcClient.right - bmp3.bmWidth, 0, rcClient.right, bmp3.bmHeight);
			AfxDrawImage(pDC, pBitmap3, rc);
			
			rc = CRect(bmp1.bmWidth, 0, rcClient.right-bmp3.bmWidth, rcClient.bottom);
			AfxDrawImage(pDC, pBitmap2, rc);
		}
	}
	else
	{
		pDC->FillRect(&rcClient, &CBrush(RGB(255, 255, 255)));
	}
	//绘icon.
	INT cx = 0;
	{
		pBitmap = m_Status == BEX_PRESSED ? m_piconSelImage : m_piconImage;
		if (pBitmap == NULL)
			pBitmap = m_piconImage;
		if (pBitmap != NULL)
		{
			CRect rx;
			GetClientRect(&rx);
			int nTop = (rx.Height() - 16)/2;
			AfxDrawImage(pDC, pBitmap, CRect(4, nTop, 20, nTop+16));
			cx = 4 + 16;
		}
	}
	//绘文本
	CString szText;
	GetWindowText(szText);
	if (!szText.IsEmpty())
	{
		CFont font;
		LOGFONT lf = {0};
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
		lstrcpy(lf.lfFaceName, m_szFont);
		lf.lfHeight= m_nFontSize;
		lf.lfWeight = m_bBold ? FW_BOLD : FW_NORMAL;
		if (m_Status == BEX_HOVER)
		{
			lf.lfUnderline = TRUE;
		}
		font.CreateFontIndirect(&lf);
		COLORREF rgb = m_clrActive;
		if (!m_bActive)
		{
			if (m_Status == BEX_HOVER)
				rgb = m_clrHover;
			else if (m_Status == BEX_PRESSED)
				rgb = m_clrSel;
			else
				rgb = m_clrText;
		}
		pDC->SetTextColor(rgb);
		pDC->SelectObject(&font);
		
		CRect rect(cx, rcClient.top, rcClient.right, rcClient.bottom);
		BOOL bSingleLine = TRUE;
		if (szText.FindOneOf(TEXT("\r\n")) != -1 || rect.Width() < pDC->GetTextExtent(szText).cy)
		{
			CRect rx = rect;
			pDC->DrawText(szText, &rx, DT_CALCRECT);
			bSingleLine = FALSE;
			int nHeight = rx.Height();
			rect.top += (rect.Height() - nHeight)/2;
			rect.bottom = rect.top + nHeight;
		}

		int nStyle = bSingleLine ? (DT_VCENTER|DT_SINGLELINE) : (DT_VCENTER);

		if (m_nBTStyle == BTEXT_LEFT)
			nStyle |= DT_LEFT;
		else if (m_nBTStyle == BTEXT_CENTER)
			nStyle |= DT_CENTER;
		else if (m_nBTStyle == BTEXT_RIGHT)
			nStyle |= DT_RIGHT;
		pDC->DrawText(szText, -1, &rect, nStyle);
	}
}

void CButtonEx::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);

	CString str;
	GetWindowText(str);
	if (m_Status != BEX_HOVER && (m_phoverImage2 != NULL || !str.IsEmpty()))
	{
		//TRACE(TEXT("mouse move.\r\n"));
		m_Status = BEX_HOVER;

		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(tme);
		tme.dwFlags = /*TME_HOVER |*/ TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::TrackMouseEvent(&tme);		
		
		Invalidate();
	}
}

void CButtonEx::OnMouseHover(UINT nFlags, CPoint point)
{
	CButton::OnMouseHover(nFlags, point);

	//TRACE(TEXT("mouse hover.\r\n"));
	Invalidate();

	TRACKMOUSEEVENT tme = {0};
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER|TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	::TrackMouseEvent(&tme);
}

void CButtonEx::OnMouseLeave()
{
	CButton::OnMouseLeave();

	//TRACE(TEXT("mouse leave.\r\n"));
	m_Status = BEX_NORMAL;
	Invalidate();
}

void CButtonEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pselectImage2 != NULL)
	{
		m_Status = BEX_PRESSED;
		Invalidate();
	}
	CButton::OnLButtonDown(nFlags, point);
}

void CButtonEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pselectImage2 != NULL)
	{
		m_Status = BEX_NORMAL;
		Invalidate();
	}

	CButton::OnLButtonUp(nFlags, point);
}

BOOL CButtonEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CButtonEx::OnEraseBkgnd(CDC* pDC)
{
	//CButton::OnEraseBkgnd(pDC);
	return TRUE;
}

IMPLEMENT_DYNAMIC(CDockProgress, CButtonEx)

BEGIN_MESSAGE_MAP(CDockProgress, CButtonEx)
END_MESSAGE_MAP()

CDockProgress::CDockProgress()
{
	m_bIsHorz   = TRUE;
	m_fProgress= 0;
	m_pbkImage	= NULL;
	m_pfgImage	= NULL;
}

CDockProgress::~CDockProgress()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pfgImage)
	{
		m_pfgImage->DeleteObject();
		delete m_pfgImage;
	}
}

void CDockProgress::SetHorzScroll(BOOL bHorz /* = TRUE */)
{
	m_bIsHorz = bHorz;
	if (m_hWnd != NULL)
		Invalidate();
}

void  CDockProgress::SetProgres(float fProgress)
{
	m_fProgress = fProgress;
	if (m_hWnd != NULL)
		Invalidate();
}

void  CDockProgress::SetImage(LPCTSTR lpszBkImage /* = NULL */, LPCTSTR lpszFgImage /* = NULL */)
{
	m_pbkImage = AfxGetSkinImage(lpszBkImage);
	m_pfgImage = AfxGetSkinImage(lpszFgImage);
}

void CDockProgress::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	if (m_pbkImage != NULL)
	{
		BITMAP bmp = {0};
		m_pbkImage->GetBitmap(&bmp);
		dcMem.SelectObject(m_pbkImage);
		pDC->StretchBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}

	if (m_pfgImage != NULL)
	{
		BITMAP bmp = {0};
		m_pbkImage->GetBitmap(&bmp);
		dcMem.SelectObject(m_pfgImage);
		if (m_bIsHorz)
		{
			int nWidth = rc.Width() * m_fProgress / 100;
			pDC->StretchBlt(0, 0, nWidth, rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
		else
		{
			int nHeight = rc.Height() * m_fProgress / 100;
			pDC->StretchBlt(0, 0, rc.Width(), nHeight, &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		}
	}
}

IMPLEMENT_DYNAMIC(CDockHtmlCtrl, CHtmlView)

BEGIN_MESSAGE_MAP(CDockHtmlCtrl, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CDockHtmlCtrl::CDockHtmlCtrl()
{
	m_bScroll = TRUE;
}

CDockHtmlCtrl::~CDockHtmlCtrl()
{

}

// BEGIN_INTERFACE_MAP(CDockHtmlCtrl, CHtmlView)
// 	INTERFACE_PART(CDockHtmlCtrl, IID_IOleCommandTarget, OleCommandTarget)
// END_INTERFACE_MAP()
// 
// STDMETHODIMP_(ULONG) CDockHtmlCtrl::XOleCommandTarget::AddRef()
// {
// 	METHOD_PROLOGUE_EX_(CDockHtmlCtrl, OleCommandTarget)
// 		return pThis->ExternalAddRef();
// }
// 
// STDMETHODIMP_(ULONG) CDockHtmlCtrl::XOleCommandTarget::Release()
// {
// 	METHOD_PROLOGUE_EX_(CDockHtmlCtrl, OleCommandTarget)
// 		return pThis->ExternalRelease();
// }
// 
// STDMETHODIMP CDockHtmlCtrl::XOleCommandTarget::QueryInterface(
// 	REFIID iid, LPVOID far* ppvObj)     
// {
// 	METHOD_PROLOGUE_EX_(CDockHtmlCtrl, OleCommandTarget)
// 		return pThis->ExternalQueryInterface(&iid, ppvObj);
// }
// 
// STDMETHODIMP CDockHtmlCtrl::XOleCommandTarget::QueryStatus( const GUID *pguidCmdGroup,
// 	ULONG cCmds, OLECMD *prgCmds,OLECMDTEXT *pCmdText)
// {
// 	return S_OK;
// }
// 
// STDMETHODIMP CDockHtmlCtrl::XOleCommandTarget::Exec(const GUID *pguidCmdGroup, DWORD nCmdID,
// 	DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut)
// {
// 	return S_OK;
// }

void CDockHtmlCtrl::SetNoScroll()
{
	m_bScroll = FALSE;
}

void CDockHtmlCtrl::OnTitleChange(LPCTSTR lpszText)
{
	// 	OutputDebugString(lpszText);
	// 	OutputDebugString(TEXT("\r\n"));
}

HRESULT CDockHtmlCtrl::OnGetHostInfo(DOCHOSTUIINFO *info)
{
	//控件IE控件的边框，滚动条有无，以及扁平滚动条
	info->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR;
	if (!m_bScroll)
	{
		info->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
	}
	return S_OK;
}

void CDockHtmlCtrl::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	*pbCancel = TRUE;
}

HRESULT CDockHtmlCtrl::OnShowContextMenu(DWORD dwID, LPPOINT ppt,
										 LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
{
	return S_OK;
}

int  CDockHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CDockHtmlCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{
}

CEditEx::~CEditEx()
{

}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CEditEx::xProcess()
{
	CString str;
	GetWindowText(str);
	if (str.IsEmpty())
	{
		SetWindowText(m_strDef);
	}
}

BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd != m_hWnd)
		return CEdit::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_CHAR)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			//return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void CEditEx::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();
}

void CEditEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);

	CString str;
	GetWindowText(str);
	if (str.CompareNoCase(m_strDef) == 0)
	{
		SetWindowText(TEXT(""));
	}
}

void CEditEx::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	xProcess();
}

IMPLEMENT_DYNAMIC(CDockComboBox, CComboBox)

CDockComboBox::CDockComboBox()
{
	m_pbkImage	= AfxGetSkinImage(TEXT("Skin/BrowseBar/背景_地址栏_1px.png"));
	m_pRight	= AfxGetSkinImage(TEXT("Skin/BrowseBar/按钮_地址栏_下拉列表_默认状态.png"));
	m_Edit.SetDefaultText(DEFAULT_TIP_TEXT2);
}

CDockComboBox::~CDockComboBox()
{
	if (m_pRight)
	{
		m_pRight->DeleteObject();
		delete m_pRight;
	}

	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

BEGIN_MESSAGE_MAP(CDockComboBox, CComboBox)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CDockComboBox::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.bottom = rcClient.top + 25;

	CRect rx = rcClient;	
	AfxDrawImage(&dc, m_pbkImage, rcClient);

	rx = rcClient;
	rx.left = rcClient.right - 26;	
	AfxDrawImage(&dc, m_pRight, rx);
}

void CDockComboBox::OnSize(UINT nType, int cx, int cy)
{
	CComboBox::OnSize(nType, cx, cy);

	if (m_Edit.GetSafeHwnd() != NULL)
	{
		CRect rc;
		m_Edit.GetWindowRect(&rc);
		ScreenToClient(&rc);
		if (rc.top < 5)
		{
			rc.OffsetRect(0, 5);
			m_Edit.MoveWindow(&rc);
		}
	}
}

void CDockComboBox::OnDestroy()
{
	if (m_Edit.GetSafeHwnd() != NULL)
		m_Edit.UnsubclassWindow();
	CComboBox::OnDestroy();
}

HBRUSH CDockComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		if ( m_Edit.GetSafeHwnd() == NULL )
		{
			m_Edit.SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CDockComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDockComboBox::OnSetFocus(CWnd* pOldWnd)
{
	m_Edit.SetFocus();
}

CDockNavBar::CDockNavBar()
{
	m_bIsDockMini = FALSE;
	m_pbkImage		= AfxGetSkinImage(TEXT("Skin/Dock/背景1像素.png"));
	m_pbkLogoImage	= AfxGetSkinImage(TEXT("Skin/Dock/logo.png"));
	m_pbkTipImage	= AfxGetSkinImage(TEXT("Skin/Dock/关闭按钮_背景.png"));
	m_nActive = -1;
	for (int idx=0; idx<_countof(m_NavClicks); idx++)
	{
		m_NavClicks[idx].dwIdx = idx+1;
		m_NavClicks[idx].dwClick = 0;
		m_NavClicks[idx].dwRunTime = 0;
	}
}

CDockNavBar::~CDockNavBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pbkLogoImage)
	{
		m_pbkLogoImage->DeleteObject();
		delete m_pbkLogoImage;
	}
	if (m_pbkTipImage)
	{
		m_pbkTipImage->DeleteObject();
		delete m_pbkTipImage;
	}
}

IMPLEMENT_DYNAMIC(CDockNavBar, CStatic)

BEGIN_MESSAGE_MAP(CDockNavBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_NAVBAR_INDEX, DOCK_NAVBAR_INDEX + 0xff, OnClickButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDockNavBar::SetActive(int idx)
{
	for (int loop=0; loop<6; loop++)
	{
		if (loop == idx)
			m_btnNav[2*loop].SetActive(TRUE);
		else
			m_btnNav[2*loop].SetActive(FALSE);
	}
	m_nActive = idx;
	if (idx>=0 && idx<_countof(m_NavClicks))
		m_NavClicks[idx].dwClick++;
}

void CDockNavBar::SetAllUnActive()
{
	for (int loop=0; loop<6; loop++)
	{
		m_btnNav[2*loop].SetActive(FALSE);
	}
	m_nActive = -1;
}

void CDockNavBar::SetDockMini(BOOL bIsMini)
{
	m_bIsDockMini = bIsMini;
	if (m_btnNetBar.m_hWnd != NULL)
	{
		PostMessage(WM_SIZE);
		m_btnNetBar.ShowWindow(!bIsMini);
		for (int idx=0; idx<_countof(m_btnTool); idx++)
			m_btnTool[idx].ShowWindow(!bIsMini);
		RedrawWindow();		
	}
}

void CDockNavBar::SetNetBarText(CString& str)
{
	m_btnNetBar.SetWindowText(str);
}

void CDockNavBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	if (!m_bIsDockMini)
	{
		CRect rx = rc;
		rx.right = 65;
		AfxDrawImage(pDC, m_pbkLogoImage, rx);

		rx = rc;
		rx.left = rc.right - 241;
		rx.bottom = 27;
		AfxDrawImage(pDC, m_pbkTipImage, rx);

		rx.top = rx.top;
		rx.bottom = rc.bottom;
		AfxDrawImage(pDC, m_pbkImage, rx);

		//6个导航栏按钮两边的拉伸(6个按钮的宽度(82+2)*6-2=502)
		rx = rc;
		rx.right = rc.right - 241;
		rx.left = (rc.Width() + 502)/2;
		AfxDrawImage(pDC,m_pbkImage, rx);

		rx.left = 65+206;
		rx.right = (rc.Width() - 502)/2;
		AfxDrawImage(pDC, m_pbkImage, rx);
	}
	else
	{
		CRect rx = rc;
		rx.right = 10;
		AfxDrawImage(pDC, m_pbkImage, rx);

		rx = rc;
		rx.left = rc.right - 10;
		AfxDrawImage(pDC, m_pbkImage, rx);
	}
}

int CDockNavBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	

	CRect rc(0, 0, 0, 0);
	m_btnNetBar.Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 0xffff);
	m_btnNetBar.SetImage(TEXT("Skin/Dock/网吧名称_背景_默认状态.png"), TEXT("Skin/Dock/网吧名称_背景_鼠标经过.png"), 
		TEXT("Skin/Dock/网吧名称_背景_鼠标按下.png"));
	m_btnNetBar.SetTextStyle(DEFAULT_FONT_NAME, 12, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, CButtonEx::BTEXT_CENTER);
	m_btnNetBar.SetCursor(hCursor);
	
	for (int idx=0; idx<6; idx++)
	{
		m_btnNav[2*idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 
			DOCK_NAVBAR_INDEX + idx);
		m_btnNav[2*idx+1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 0xffff);
		m_btnNav[2*idx+1].SetImage(TEXT("Skin/Dock/背景_头部_按钮分割线.png"));
		m_btnNav[2*idx].SetCursor(hCursor);
	}
	m_btnNav[0].SetImage(TEXT("Skin/Dock/按钮_浏览器_默认状态.png"), TEXT("Skin/Dock/按钮_浏览器_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_浏览器_鼠标按下.png"), TEXT("Skin/Dock/按钮_浏览器_激活.png"));
	m_btnNav[2].SetImage(TEXT("Skin/Dock/按钮_游戏大厅_默认状态.png"), TEXT("Skin/Dock/按钮_游戏大厅_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_游戏大厅_鼠标按下.png"), TEXT("Skin/Dock/按钮_游戏大厅_激活.png"));
	m_btnNav[4].SetImage(TEXT("Skin/Dock/按钮_聊天工具_默认状态.png"), TEXT("Skin/Dock/按钮_聊天工具_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_聊天工具_鼠标按下.png"), TEXT("Skin/Dock/按钮_聊天工具_激活.png"));
	m_btnNav[6].SetImage(TEXT("Skin/Dock/按钮_常用软件_默认状态.png"), TEXT("Skin/Dock/按钮_常用软件_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_常用软件_鼠标按下.png"), TEXT("Skin/Dock/按钮_常用软件_激活.png"));
	m_btnNav[8].SetImage(TEXT("Skin/Dock/按钮_点卡商城_默认状态.png"), TEXT("Skin/Dock/按钮_点卡商城_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_点卡商城_鼠标按下.png"), TEXT("Skin/Dock/按钮_点卡商城_激活.png"));
	m_btnNav[10].SetImage(TEXT("Skin/Dock/按钮_影视音乐_默认状态.png"), TEXT("Skin/Dock/按钮_影视音乐_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_影视音乐_鼠标按下.png"), TEXT("Skin/Dock/按钮_影视音乐_激活.png"));

	DWORD dwID = DOCK_NAVBAR_INDEX + 6;
	m_btnTool[0].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[2].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[3].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[4].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[5].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);

	m_btnTool[0].SetImage(TEXT("Skin/Dock/按钮_缩小_默认.png"), TEXT("Skin/Dock/按钮_缩小_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_缩小_按下.png"));
	m_btnTool[1].SetImage(TEXT("Skin/Dock/按钮_扩大_默认.png"), TEXT("Skin/Dock/按钮_扩大_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_扩大_按下.png"));
	m_btnTool[2].SetImage(TEXT("Skin/Dock/按钮_关闭_默认.png"), TEXT("Skin/Dock/按钮_关闭_鼠标经过.png"),
		TEXT("Skin/Dock/按钮_关闭_按下.png"));

	m_btnTool[3].SetImage(TEXT("Skin/Dock/按钮_个性设置_默认状态.png"), 
		TEXT("Skin/Dock/按钮_个性设置_鼠标经过.png"), TEXT("Skin/Dock/按钮_个性设置_鼠标按下.png"));
	m_btnTool[4].SetImage(TEXT("Skin/Dock/按钮_个人U盘_默认状态.png"),
		TEXT("Skin/Dock/按钮_个人U盘_鼠标经过.png"), TEXT("Skin/Dock/按钮_个人U盘_鼠标按下.png"));
	m_btnTool[5].SetImage(TEXT("Skin/Dock/按钮_收藏夹_默认状态.png"),
		TEXT("Skin/Dock/按钮_收藏夹_鼠标经过.png"), TEXT("Skin/Dock/按钮_收藏夹_鼠标按下.png"));

	SetTimer(1, 1000, NULL);
	return 0;
}

BOOL CDockNavBar::OnEraseBkgnd(CDC* pDC)
{
	//return CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

void CDockNavBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	
	CRect rcClient, rc;
	GetClientRect(&rcClient);
	rc.bottom = 59;
	rc = rcClient;

	if (!m_bIsDockMini)
	{
		//netbar.
		rc.left = 65;
		rc.right = rc.left + 206;
		m_btnNetBar.MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);

		//让导航栏剧中
		rc.left = (rcClient.Width() - 502)/2;
		if (rc.left<65+206)
			rc.left = 65+206;
	}
	else
	{
		rc.left = 10;
	}
	rc.right = rc.left + 82;
	for (int idx=0; idx<6; idx++)
	{
		m_btnNav[idx*2].MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);
		if (idx < 5)
		{
			m_btnNav[idx*2+1].MoveWindow(CRect(rc.left, rc.top, rc.left + 2, rc.bottom));
			rc.OffsetRect(2, 0);
		}
	}

	if (!m_bIsDockMini)
	{
		//tool.
		rc.top = 4;
		rc.left = rcClient.right- 75;
		rc.right = rc.left + 22;
		rc.bottom = rc.top + 20;
		m_btnTool[0].MoveWindow(&rc);

		rc.OffsetRect(rc.Width(), 0);
		rc.right = rc.left + 31;
		m_btnTool[1].MoveWindow(&rc);

		rc.OffsetRect(rc.Width(), 0);
		rc.right = rc.left + 17;
		m_btnTool[2].MoveWindow(&rc);

		//return ;
		rc = rcClient;
		rc.left = rc.right - 241;
		rc.right = rc.left + 104;
		rc.top = 27; rc.bottom = 59;
		m_btnTool[3].MoveWindow(&rc);

		rc.OffsetRect(rc.Width()+3, 0);
		rc.right = rc.left + 63;
		m_btnTool[4].MoveWindow(&rc);

		rc.OffsetRect(rc.Width()+3, 0);
		rc.right = rc.left + 63;
		m_btnTool[5].MoveWindow(&rc);
	}
}

void CDockNavBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		DWORD dwId = id - DOCK_NAVBAR_INDEX;
		pParent->SendMessage(WM_NAV_CLICK_MESSAGE, dwId, 0);
	}
}

void CDockNavBar::OnTimer(UINT_PTR nIDEvent)
{
	CStatic::OnTimer(nIDEvent);
	int nSel = GetActive();
	if (nSel == -1 || m_bIsDockMini)
		return ;

	m_NavClicks[nSel].dwRunTime++;
// #ifdef _DEBUG
// 	OutputDebugString(TEXT("\r\n"));
// 	for (int idx=0; idx<_countof(m_NavClicks); idx++)
// 	{
// 		TCHAR szLog[MAX_PATH] = {0};
// 		_stprintf(szLog, TEXT("Idx=%d, Click=%d, RunTime=%d; "), m_NavClicks[idx].dwIdx, 
// 			m_NavClicks[idx].dwClick, m_NavClicks[idx].dwRunTime);
// 		OutputDebugString(szLog);
// 	}
// 	OutputDebugString(TEXT("\r\n"));
// #endif // _DEBUG
}

void CDockNavBar::GetAllNavClick(tagNavClick* pNavClick)
{
	for (int idx=0; idx<_countof(m_NavClicks); idx++)
	{
		pNavClick[idx].dwIdx = m_NavClicks[idx].dwIdx;
		pNavClick[idx].dwClick = m_NavClicks[idx].dwClick;
		pNavClick[idx].dwRunTime = m_NavClicks[idx].dwRunTime;

		m_NavClicks[idx].dwClick  = 0;
		m_NavClicks[idx].dwRunTime = 0;
	}
}

IMPLEMENT_DYNAMIC(CDockBrowseBar, CStatic)

BEGIN_MESSAGE_MAP(CDockBrowseBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_BROWSE_INDEX, DOCK_BROWSE_INDEX + 0xff, OnClickButton)
END_MESSAGE_MAP()

CDockBrowseBar::CDockBrowseBar()
{
	m_pbkImage = AfxGetSkinImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_1px.png"));
}

CDockBrowseBar::~CDockBrowseBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

CString CDockBrowseBar::GetText()
{
	CString str = m_CtrlUrl.GetText();
	if (str.CompareNoCase(DEFAULT_TIP_TEXT2) == 0)
		str = TEXT("");
	return str;
}

void CDockBrowseBar::SetText(CString &str)
{
	if (m_CtrlUrl.GetSafeHwnd() != NULL)
	{
		CString old = GetText();
		if (str.CompareNoCase(old) != 0)
		{
			m_CtrlUrl.SetWindowText(str);
		}
	}
}

void CDockBrowseBar::SetListText(std::vector<CString>& lst)
{
	if (m_CtrlUrl.GetSafeHwnd() != NULL)
	{
		m_CtrlUrl.ResetContent();
		for (size_t idx=0; idx<lst.size(); idx++)
		{
			m_CtrlUrl.AddString(lst[idx]);
		}
	}
}

BOOL CDockBrowseBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnClickButton(DOCK_BROWSE_INDEX + 10);
		return TRUE;
	}
	return CStatic::PreTranslateMessage(pMsg);
}

void CDockBrowseBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	AfxDrawImage(pDC, m_pbkImage, rc);
}

BOOL CDockBrowseBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int  CDockBrowseBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_BROWSE_INDEX + idx);
		m_BtnGroup[idx].SetCursor(hCursor);
	}

	m_BtnGroup[0].SetImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_左.png"));
	m_BtnGroup[1].SetImage(TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网页收藏夹_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网页收藏夹_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网页收藏夹_按下.png"));

	m_BtnGroup[2].SetImage(TEXT("Skin/BrowseBar/按钮_浏览器工具栏_后退_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_浏览器工具栏_后退_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_浏览器工具栏_后退_按下.png"));
	m_BtnGroup[3].SetImage(TEXT("Skin/BrowseBar/按钮_浏览器工具栏_前进_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_浏览器工具栏_前进_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_浏览器工具栏_前进_按下.png"));
	m_BtnGroup[4].SetImage(TEXT("Skin/BrowseBar/按钮_浏览器工具栏_首页_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_浏览器工具栏_首页_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_浏览器工具栏_首页_按下.png"));

	m_BtnGroup[5].SetImage(TEXT("Skin/BrowseBar/背景_地址栏_左.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/BrowseBar/背景_地址栏_1px.png"));
	m_CtrlUrl.Create(CBS_DROPDOWN|WS_CHILD|CBS_HASSTRINGS|WS_VISIBLE|CBS_AUTOHSCROLL, CRect(0, 0, 0, 0), &m_BtnGroup[6], DOCK_BROWSE_INDEX + sizeof(m_BtnGroup));
	m_CtrlUrl.AddString(TEXT(""));
	m_CtrlUrl.AddString(TEXT(""));
	m_CtrlUrl.AddString(TEXT(""));

	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_font.CreateFontIndirect(&lf);
	m_CtrlUrl.SetFont(&m_font);
	m_CtrlUrl.SetWindowText(DEFAULT_TIP_TEXT2);

	m_BtnGroup[7].SetImage(TEXT("Skin/BrowseBar/按钮_地址栏_下拉列表_默认状态.png"));
	m_BtnGroup[8].SetImage(TEXT("Skin/BrowseBar/按钮_地址栏_刷新_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_地址栏_刷新_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_地址栏_刷新_按下.png"));
	m_BtnGroup[9].SetImage(TEXT("Skin/BrowseBar/按钮_地址栏_关闭_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_地址栏_关闭_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_地址栏_关闭_按下.png"));
	m_BtnGroup[10].SetImage(TEXT("Skin/BrowseBar/按钮_地址栏_GO_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_地址栏_GO_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_地址栏_GO_按下.png"));
	
	m_BtnGroup[11].SetImage(TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网址导航_默认状态.png"), 
		TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网址导航_鼠标经过.png"), TEXT("Skin/BrowseBar/按钮_浏览器工具栏_网址导航_按下.png"));
	m_BtnGroup[12].SetImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_右.png"));

	return 0;
}

void CDockBrowseBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	if (m_BtnGroup[0].m_hWnd == NULL)
		return ;

	//左边按钮
	CRect rc(0, 0, 6, 37);
	m_BtnGroup[0].MoveWindow(&rc);

	rc.OffsetRect(rc.Width(), 6);
	rc.bottom -= 12;
	rc.right = rc.left + 94;
	m_BtnGroup[1].MoveWindow(&rc);
	
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 34;
	m_BtnGroup[2].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	m_BtnGroup[3].MoveWindow(&rc);
	rc.OffsetRect(rc.Width()+4, 0);
	rc.right = rc.left + 32;
	m_BtnGroup[4].MoveWindow(&rc);

	//地址栏
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 30;
	m_BtnGroup[5].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		rc.right = rc.left + rcClient.Width() - (1024 - 530) + 26;
	}
	m_BtnGroup[6].MoveWindow(&rc);
	{
		CRect rx;
		m_BtnGroup[6].GetClientRect(&rx);
		m_CtrlUrl.MoveWindow(&rx);
	}
// 	rc.OffsetRect(rc.Width(), 0);
// 	rc.right = rc.left + 26;
// 	m_BtnGroup[7].MoveWindow(&rc);	//下拉
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 26;
	m_BtnGroup[8].MoveWindow(&rc);	//刷新
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 26;
	m_BtnGroup[9].MoveWindow(&rc);	//停止
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 65;
	m_BtnGroup[10].MoveWindow(&rc);	//GO.

	//网址导航按钮
	rc.OffsetRect(rc.Width()+4, 0);
	rc.right = rc.left + 99;
	m_BtnGroup[11].MoveWindow(&rc);

	rc.OffsetRect(rc.Width(), 0);		//右边界线
	rc.right = rc.left + 6;
	rc.top = 0; rc.bottom +=6;
	m_BtnGroup[12].MoveWindow(&rc);
}

void CDockBrowseBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		id = id - DOCK_BROWSE_INDEX;
		pParent->SendMessage(WM_BROWSE_CLICK_MESSAGE, id, 0);
	}
}

IMPLEMENT_DYNAMIC(CDockFinderBar, CStatic)

BEGIN_MESSAGE_MAP(CDockFinderBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_FINDER_INDEX, DOCK_FINDER_INDEX + 0xff, OnClickButton)
END_MESSAGE_MAP()

CDockFinderBar::CDockFinderBar()
{
	m_pbkImage		= AfxGetSkinImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_1px.png"));
	m_pbkLeft		= AfxGetSkinImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_左.png"));
	m_pbkRight		= AfxGetSkinImage(TEXT("Skin/BrowseBar/背景_浏览器工具栏_右.png"));
	m_pbAlpha		= AfxGetSkinImage(TEXT("Skin/FinderBar/背景_字母背景_1px.png"));
	m_pbAlphaLeft	= AfxGetSkinImage(TEXT("Skin/FinderBar/背景_字母背景_左.png"));
	m_pbAlphaRight	= AfxGetSkinImage(TEXT("Skin/FinderBar/背景_字母背景_右.png"));
}

CDockFinderBar::~CDockFinderBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pbkLeft)
	{
		m_pbkLeft->DeleteObject();
		delete m_pbkLeft;
	}
	if (m_pbkRight)
	{
		m_pbkRight->DeleteObject();
		delete m_pbkRight;
	}
	if (m_pbAlpha)
	{
		m_pbAlpha->DeleteObject();
		delete m_pbAlpha;
	}
	if (m_pbAlphaLeft)
	{
		m_pbAlphaLeft->DeleteObject();
		delete m_pbAlphaLeft;
	}
	if (m_pbAlphaRight)
	{
		m_pbAlphaRight->DeleteObject();
		delete m_pbAlphaRight;
	}
}

CString CDockFinderBar::GetText()
{
	CString str;
	m_edit.GetWindowText(str);
	if (str.CompareNoCase(DEFAULT_TIP_TEXT1) == 0)
		str = TEXT("");
	return str;
}

BOOL CDockFinderBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYUP)
		{
			OnClickButton(DOCK_FINDER_INDEX+2);
		}
	}
	return CStatic::PreTranslateMessage(pMsg);
}

void CDockFinderBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	
	CRect rx = rc;
	rx.right = 6;
	AfxDrawImage(pDC, m_pbkLeft, rx);

	rx.left =  6;
	rx.right = rc.right - 6;
	AfxDrawImage(pDC, m_pbkImage, rx);

	rx.left = rc.right - 6;
	rx.right = rc.right;
	AfxDrawImage(pDC, m_pbkRight, rx);
	
	rx = CRect(rc.right-613-6, 6, rc.right-6, rc.bottom - 6);
	AfxDrawImage(pDC, m_pbAlpha, rx);

	rx.right = rx.left + 5;
	AfxDrawImage(pDC, m_pbAlphaLeft, rx);

	rx.left = rc.right - 11;
	rx.right = rx.left + 5;
	AfxDrawImage(pDC, m_pbAlphaRight, rx);
}

BOOL CDockFinderBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int  CDockFinderBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_FINDER_INDEX + idx);
		if (idx>=2)
			m_BtnGroup[idx].SetCursor(hCursor);
	}

	//文本框
	m_BtnGroup[0].SetImage(TEXT("Skin/FinderBar/背景_快速查找_左.png"));
	m_BtnGroup[1].SetImage(TEXT("Skin/FinderBar/背景_快速查找_1px.png"));
	m_edit.SetDefaultText(DEFAULT_TIP_TEXT1);
	m_edit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, CRect(0, 0, 0, 0), &m_BtnGroup[1], DOCK_FINDER_INDEX + sizeof(m_BtnGroup));
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_font.CreateFontIndirect(&lf);
	m_edit.SetFont(&m_font);
	m_edit.SetWindowText(DEFAULT_TIP_TEXT1);
	m_BtnGroup[2].SetImage(TEXT("Skin/FinderBar/按钮_GO_默认状态.png"), 
		TEXT("Skin/FinderBar/按钮_GO_鼠标经过.png"), TEXT("Skin/FinderBar/按钮_GO_按下.png"));

	//字线表
	m_BtnGroup[3].SetImage(TEXT("Skin/FinderBar/a_默认状态.png"), TEXT("Skin/FinderBar/a_激活.png"));
	m_BtnGroup[4].SetImage(TEXT("Skin/FinderBar/b_默认状态.png"), TEXT("Skin/FinderBar/b_激活.png"));
	m_BtnGroup[5].SetImage(TEXT("Skin/FinderBar/c_默认状态.png"), TEXT("Skin/FinderBar/c_激活.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/FinderBar/d_默认状态.png"), TEXT("Skin/FinderBar/d_激活.png"));
	m_BtnGroup[7].SetImage(TEXT("Skin/FinderBar/e_默认状态.png"), TEXT("Skin/FinderBar/e_激活.png"));
	m_BtnGroup[8].SetImage(TEXT("Skin/FinderBar/f_默认状态.png"), TEXT("Skin/FinderBar/f_激活.png"));
	m_BtnGroup[9].SetImage(TEXT("Skin/FinderBar/g_默认状态.png"), TEXT("Skin/FinderBar/g_激活.png"));
	m_BtnGroup[10].SetImage(TEXT("Skin/FinderBar/h_默认状态.png"), TEXT("Skin/FinderBar/h_激活.png"));
	m_BtnGroup[11].SetImage(TEXT("Skin/FinderBar/i_默认状态.png"), TEXT("Skin/FinderBar/i_激活.png"));
	m_BtnGroup[12].SetImage(TEXT("Skin/FinderBar/j_默认状态.png"), TEXT("Skin/FinderBar/j_激活.png"));
	m_BtnGroup[13].SetImage(TEXT("Skin/FinderBar/k_默认状态.png"), TEXT("Skin/FinderBar/k_激活.png"));

	m_BtnGroup[14].SetImage(TEXT("Skin/FinderBar/l_默认状态.png"), TEXT("Skin/FinderBar/l_激活.png"));
	m_BtnGroup[15].SetImage(TEXT("Skin/FinderBar/m_默认状态.png"), TEXT("Skin/FinderBar/m_激活.png"));
	m_BtnGroup[16].SetImage(TEXT("Skin/FinderBar/n_默认状态.png"), TEXT("Skin/FinderBar/n_激活.png"));
	m_BtnGroup[17].SetImage(TEXT("Skin/FinderBar/o_默认状态.png"), TEXT("Skin/FinderBar/o_激活.png"));
	m_BtnGroup[18].SetImage(TEXT("Skin/FinderBar/p_默认状态.png"), TEXT("Skin/FinderBar/p_激活.png"));
	m_BtnGroup[19].SetImage(TEXT("Skin/FinderBar/q_默认状态.png"), TEXT("Skin/FinderBar/q_激活.png"));
	m_BtnGroup[20].SetImage(TEXT("Skin/FinderBar/r_默认状态.png"), TEXT("Skin/FinderBar/r_激活.png"));
	m_BtnGroup[21].SetImage(TEXT("Skin/FinderBar/s_默认状态.png"), TEXT("Skin/FinderBar/s_激活.png"));
	m_BtnGroup[22].SetImage(TEXT("Skin/FinderBar/t_默认状态.png"), TEXT("Skin/FinderBar/t_激活.png"));
	
	m_BtnGroup[23].SetImage(TEXT("Skin/FinderBar/u_默认状态.png"), TEXT("Skin/FinderBar/u_激活.png"));
	m_BtnGroup[24].SetImage(TEXT("Skin/FinderBar/v_默认状态.png"), TEXT("Skin/FinderBar/v_激活.png"));
	m_BtnGroup[25].SetImage(TEXT("Skin/FinderBar/w_默认状态.png"), TEXT("Skin/FinderBar/w_激活.png"));
	m_BtnGroup[26].SetImage(TEXT("Skin/FinderBar/x_默认状态.png"), TEXT("Skin/FinderBar/x_激活.png"));
	m_BtnGroup[27].SetImage(TEXT("Skin/FinderBar/y_默认状态.png"), TEXT("Skin/FinderBar/y_激活.png"));
	m_BtnGroup[28].SetImage(TEXT("Skin/FinderBar/z_默认状态.png"), TEXT("Skin/FinderBar/z_激活.png"));
	for (int idx=3; idx<29;idx++)
	{
		HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
		m_BtnGroup[idx].SetCursor(hCursor);
	}

	return 0;
}

void CDockFinderBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (m_BtnGroup[0].m_hWnd == NULL)
		return ;

	CRect rcClient;
	GetClientRect(&rcClient);

	//快速查找
	CRect rc(6, 6, 102+6, 31);
	m_BtnGroup[0].MoveWindow(&rc);

	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + (rcClient.Width() - 780);	// 603 + 5 + 5 + 4 + 6+ 6+ 49 + 102.
	m_BtnGroup[1].MoveWindow(&rc);
	{
		CRect rx;
		m_BtnGroup[1].GetClientRect(&rx);
		rx.DeflateRect(5, 7, 5, 5);
		m_edit.MoveWindow(&rx);
	}
	
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 49;
	m_BtnGroup[2].MoveWindow(&rc);

	//字线表
	rc.left = rcClient.right - 6 - 603 - 5;
	int width_alp[] = {26, 22, 24, 25, 22, 21, 25, 24, 16, 21, 23, 21, 27, 26, 25, 22, 26, 23, 22, 23, 24, 21, 28, 23, 23, 20};
	int nWid = 0;
	for (int idx=0; idx<_countof(width_alp); idx++)
	{
		rc.right = rc.left + width_alp[idx];
		m_BtnGroup[idx+3].MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);
		nWid +=width_alp[idx];
	}
}

void CDockFinderBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		id -= DOCK_FINDER_INDEX;
		LPARAM param = 0;
		if (id > 2 && id < 29)
		{			
			CString str;
			//m_edit.GetWindowText(str);
			//if (str.CompareNoCase(m_edit.GetDefaultText()) == 0)
			//	  str = TEXT("");
			str += TCHAR(TEXT('A') + id - 3);
			m_edit.SetWindowText(str);
			id = 2;
			param = 1;
		}
		pParent->SendMessage(WM_FINDER_CLICK_MESSAGE, id , param);
	}
}

CDockTabBar::CDockTabBar()
{
	m_pbkImage = AfxGetSkinImage(TEXT("Skin/TabBar/背景_标签栏_1px.png"));
	m_nActive = -1;
}

CDockTabBar::~CDockTabBar()
{
	RemoveAllTab();
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

IMPLEMENT_DYNAMIC(CDockTabBar, CStatic)

BEGIN_MESSAGE_MAP(CDockTabBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_TAB_INDEX, DOCK_TAB_INDEX+16, OnClickButton)
END_MESSAGE_MAP()

CButtonEx* CDockTabBar::AddTab(LPCTSTR lpszCap, LPCTSTR iconImage, LPCTSTR iconSelImage, BOOL bRefresh /* = FALSE */)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	CButtonEx* pTab = new CButtonEx;
	pTab->Create(TEXT("Tab"), BS_CENTER|WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, DOCK_TAB_INDEX+1+m_BtnTabGroup.size());
	pTab->SetImage(TEXT("Skin/TabBar/背景_未选中标签.png"), TEXT("Skin/TabBar/背景_未选中标签.png"),
		TEXT("Skin/TabBar/背景_选中标签.png"), TEXT("Skin/TabBar/背景_选中标签.png"));
	pTab->SetWindowText(lpszCap);
	pTab->SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 216, 0), RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER);
	pTab->SetIconImage(iconImage, iconSelImage);
	m_BtnTabGroup.push_back(pTab);
	pTab->SetCursor(hCursor);
	if (bRefresh)
		PostMessage(WM_SIZE);
	return pTab;
}

CString  CDockTabBar::GetTabText(int index)
{
	CString str;
	if (m_BtnTabGroup.size() > (size_t)index)
	{
		m_BtnTabGroup[index]->GetWindowText(str);
	}
	return str;
}
void CDockTabBar::RemoveAllTab()
{
	for (std::vector<CButtonEx*>::iterator it = m_BtnTabGroup.begin(); 
		it != m_BtnTabGroup.end(); it++)
	{
		CButtonEx* pTab = *it;
		if (pTab != NULL)
		{
			pTab->DestroyWindow();
			delete pTab;
		}
	}
	if (m_hWnd != NULL)
		PostMessage(WM_SIZE);
	m_BtnTabGroup.clear();
}

void CDockTabBar::ShowUrlNav(BOOL bShow)
{
	if (m_btnFind.GetSafeHwnd() != NULL)
		m_btnFind.ShowWindow(bShow);
}

void CDockTabBar::SetActive(int idx)
{
	for (size_t loop=0; loop<m_BtnTabGroup.size(); loop++)
	{
		if (idx == loop)
			m_BtnTabGroup[loop]->SetActive(TRUE);
		else
			m_BtnTabGroup[loop]->SetActive(FALSE);
	}
	m_nActive = idx;
}

void CDockTabBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	AfxDrawImage(pDC, m_pbkImage, rc);
	return ;
}

BOOL CDockTabBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int CDockTabBar:: OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	m_btnBoderLeft.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 6, 32), this, 0xffff);
	m_btnBorderRight.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, 0xffff);
	m_btnBoderLeft.SetImage(TEXT("Skin/TabBar/背景_标签栏_左.png"));
	m_btnBorderRight.SetImage(TEXT("Skin/TabBar/背景_标签栏_右.png"));

	m_btnFind.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_TAB_INDEX);
	m_btnFind.SetImage(TEXT("Skin/TabBar/选游戏_未选中状态.png"), NULL, TEXT("Skin/TabBar/选游戏_选中状态.png"));
	m_btnFind.SetCursor(hCursor);

	return 0;
}

void CDockTabBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	
	if (m_btnFind.m_hWnd == NULL)
		return ;

	CRect rcClient, rc;
	GetClientRect(&rcClient);
	rc = rcClient;
	rc.right = 6;
	m_btnBoderLeft.MoveWindow(&rc);

	rc = CRect(6, 2, 91, 29);
	for (std::vector<CButtonEx*>::iterator it = m_BtnTabGroup.begin(); 
		it != m_BtnTabGroup.end(); it++)
	{
		CButtonEx* pTab = *it;
		if (pTab == NULL || pTab->m_hWnd == NULL)
			continue;

		int nHeight = rc.Height();
		if (rc.right > rcClient.right-7)
			break;
		pTab->MoveWindow(&rc);
		rc.OffsetRect(rc.Width()+3, 0);
	}

// 	rc.left = rcClient.right - 130; rc.right = rc.left + 124;
// 	m_btnFind.MoveWindow(&rc);

	rc.left = rcClient.right - 6; 
	rc.top = 0;
	rc.right = rcClient.right;
	rc.bottom = 32;
	m_btnBorderRight.MoveWindow(&rc);
}

void CDockTabBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		pParent->SendMessage(WM_TAB_CLICK_MESSAGE, id - DOCK_TAB_INDEX, 0);
	}
}

IMPLEMENT_DYNAMIC(CDockStatusBar, CStatic)

CDockStatusBar::CDockStatusBar()
{
	m_pbkImage = AfxGetSkinImage(TEXT("Skin/StatusBar/背景_状态栏_1px.png"));
	m_pQuickImage = AfxGetSkinImage(TEXT("Skin/StatusBar/背景_状态栏_左.png"));
}

CDockStatusBar::~CDockStatusBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pQuickImage)
	{
		m_pQuickImage->DeleteObject();
		delete m_pQuickImage;
	}
}

void CDockStatusBar::SetQuickRunGroup(int nIndex, HICON hIcon)
{
	if (nIndex <0 || nIndex > 4)
		return ;

	m_BtnGroup[nIndex].SetIconImage(hIcon);

	PostMessage(WM_SIZE);
}

BEGIN_MESSAGE_MAP(CDockStatusBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_STABAR_INDEX, DOCK_STABAR_INDEX + 0xff, OnClickButton)
END_MESSAGE_MAP()

void CDockStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	AfxDrawImage(pDC, m_pbkImage, rc);
	rc.right = rc.left + 65;
	AfxDrawImage(pDC, m_pQuickImage, rc);
}

BOOL CDockStatusBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int  CDockStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_STABAR_INDEX+idx);
		if (idx<5)
			m_BtnGroup[idx].SetImage(TEXT("Skin/StatusBar/背景_状态栏_16x16图标背景.png"));
	}

	m_BtnGroup[5].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_左.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_1px.png"));
	m_WebPage.SetNoScroll();
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), &m_BtnGroup[6], 0xffff);
	m_WebPage.SetSilent(TRUE);
	m_WebPage.Navigate2(TEXT("http://ads.i8.com.cn/ad/client/adbar10_wz.html "));
	m_BtnGroup[7].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_右.png"));

	m_BtnGroup[8].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_分辨率_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_分辨率_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_分辨率_鼠标按下.png"));
	m_BtnGroup[9].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_鼠标_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_鼠标_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_鼠标_鼠标按下.png"));
	m_BtnGroup[10].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_音量_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_音量_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_音量_鼠标按下.png"));

	m_BtnGroup[11].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_锁定_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_锁定_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_锁定_鼠标按下.png"));
	m_BtnGroup[12].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_工具_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_工具_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_工具_鼠标按下.png"));

	m_BtnGroup[13].SetImage(TEXT("Skin/StatusBar/背景_状态栏_右.png"));
	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].SetCursor(hCursor);
	}
	return 0;
}

void CDockStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (m_BtnGroup[0].m_hWnd == NULL)
		return ;
	CRect rcClient;
	GetClientRect(&rcClient);

	//快速启动按钮.
	CRect rc(65, 6, 89, 28);
	for (int idx=0; idx<5; idx++)
	{
		if (m_BtnGroup[idx].m_piconImage != NULL)
		{
			m_BtnGroup[idx].MoveWindow(&rc);
		}
		else
		{
			m_BtnGroup[idx].MoveWindow(&CRect(0, 0, 0, 0));
		}
		rc.OffsetRect(rc.Width()+4, 0);
	}

	//网吧公告按钮
	rc.left += 2;
	rc.right = rc.left + 88;
	m_BtnGroup[5].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rcClient.Width() - 450 + 43;
	m_BtnGroup[6].MoveWindow(&rc);
	{
		CRect rx;
		m_BtnGroup[6].GetClientRect(&rx);
		m_WebPage.MoveWindow(&rx);
	}
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 3;
	m_BtnGroup[7].MoveWindow(&rc);


	//鼠标等工具栏.
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 73;
	m_BtnGroup[8].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 62;
	m_BtnGroup[9].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 65;
	m_BtnGroup[10].MoveWindow(&rc);

	//锁定按钮
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 117;
	m_BtnGroup[11].MoveWindow(&rc);

	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 28;
	m_BtnGroup[12].MoveWindow(&rc);

	//缩放按钮
	int right = rc.right;
	GetClientRect(&rc);
	if (rc.right > right)
	{		
		rc.left = rc.right - 13;
		//m_BtnGroup[12].MoveWindow(&rc);
	}
}

void CDockStatusBar::OnClickButton(UINT id)
{
	switch (id)
	{
	case 0x2108:
		WinExec("rundll32.exe shell32.dll, Control_RunDLL desk.cpl,,3", SW_SHOWNORMAL);
		break;
	case 0x2109:
		WinExec("rundll32.exe shell32.dll, Control_RunDLL main.cpl @0", SW_SHOWNORMAL);
		break;
	case 0x210a:
		//WinExec("rundll32.exe shell32.dll, Control_RunDLL mmsys.cpl @1", SW_SHOWNORMAL);
		WinExec("Sndvol32.exe", SW_SHOWNORMAL);
		break;
	}
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		pParent->SendMessage(WM_STATUS_CLICK_MESSAGE, id - DOCK_STABAR_INDEX, 0);
	}
}

IMPLEMENT_DYNAMIC(CDockHeaderCtrl, CHeaderCtrl)

CDockHeaderCtrl::CDockHeaderCtrl()
{

}

CDockHeaderCtrl::~CDockHeaderCtrl()
{

}

BEGIN_MESSAGE_MAP(CDockHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CDockHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(1);
	CRect rect;

	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(RGB(255, 255, 255)));

	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	CFont font;
	font.CreateFontIndirect(&lf);
	dc.SelectObject(&font);
	for(int i = 0; i<GetItemCount(); i++)
	{
		TCHAR buf[256];
		HD_ITEM hditem;

		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER| DT_END_ELLIPSIS ;
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER | HDI_FORMAT;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( i, &hditem );
		GetItemRect(i, &rect);
		rect.left += 3;
		if (hditem.fmt & HDF_LEFT)
			uFormat |= DT_LEFT;
		else if (hditem.fmt & HDF_CENTER)
			uFormat |= DT_CENTER;
		else if (hditem.fmt & HDF_RIGHT)
			uFormat |= DT_RIGHT;		

		dc.DrawText(buf, &rect, uFormat);
	}
}

IMPLEMENT_DYNAMIC(CDockListCtrl, CListCtrl)

CDockListCtrl::CDockListCtrl()
{
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_headFont.CreateFontIndirect(&lf);
}

CDockListCtrl::~CDockListCtrl()
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.begin();
	for (; it != m_CellColor.end(); it++)
	{
		delete it->second;
	}
	m_CellColor.clear();
	if (m_headFont.m_hObject != NULL)
	{
		m_headFont.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CDockListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomDrawList)
END_MESSAGE_MAP()

void CDockListCtrl::SetCellColor(int nRow, int nCol, COLORREF clrText, COLORREF clrBkCol, 
							   COLORREF clrSelText, COLORREF clrSelBkCol)
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.find(MAKELONG(nRow, nCol));
	tagCellColor* pCell = NULL;
	if (it == m_CellColor.end())
	{
		pCell = new tagCellColor;
		m_CellColor.insert(make_pair(MAKELONG(nRow, nCol), pCell));
	}
	else
	{
		pCell = it->second;
	}
	pCell->clrText = clrText;
	pCell->clrBkText = clrBkCol;
	pCell->clrSelText = clrSelText;
	pCell->clrSelBkText = clrSelBkCol;
}

void CDockListCtrl::GetCellColr(int nRow, int nCol, COLORREF& clrText, COLORREF& clrBkCol, 
							  COLORREF& clrSelText, COLORREF& clrSelBkCol)
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.find(MAKELONG(nRow, nCol));

	if (it == m_CellColor.end())
	{
		//return default color;
		clrText = GetTextColor();
		clrBkCol = GetBkColor();
		clrSelText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		clrSelBkCol = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		tagCellColor* pCell = it->second;
		clrText = pCell->clrText;
		clrBkCol = pCell->clrBkText;
		clrSelText = pCell->clrSelText;
		clrSelBkCol = pCell->clrSelBkText;
	}
}

void CDockListCtrl::OnCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW*   pLVCD   =   reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);   
	*pResult = CDRF_DODEFAULT;   
	int rowIndex = static_cast<int>(pLVCD->nmcd.dwItemSpec);
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)   
	{
		if (m_headCtrl.GetSafeHwnd() == NULL && GetHeaderCtrl() != NULL && GetHeaderCtrl()->m_hWnd != NULL)
		{
			m_headCtrl.SubclassWindow(GetHeaderCtrl()->m_hWnd);
			m_headCtrl.SetFont(&m_headFont, TRUE);
		}
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{     
		*pResult = CDRF_NOTIFYSUBITEMDRAW;   
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))   
	{
		CDC* pDC	 = CDC::FromHandle(pLVCD->nmcd.hdc);
		int nItem	 = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;
		CString str  = GetItemText(nItem, nSubItem);
		COLORREF clrText = 0, clrBkText = 0, clrSelText = 0, clrSelBkText = 0;
		GetCellColr(nItem, nSubItem, clrText, clrBkText, clrSelText, clrSelBkText);

		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rect);
		rect.DeflateRect(0, 0, 0, 1);
		if(LVIS_SELECTED == GetItemState(rowIndex, LVIS_SELECTED))
		{
			pDC->SetTextColor(clrSelText);

			pDC->FillSolidRect(&rect, clrSelBkText);
		}
		else
		{
			pDC->SetTextColor(clrText);
			pDC->FillSolidRect(&rect, clrBkText);
		}

		if (nSubItem > 0)
			rect.left += 3;
		LV_COLUMN lv = {0};
		lv.mask = LVCF_FMT;
		GetColumn(nSubItem, &lv);
		DWORD nStyle = DT_VCENTER;
		if (lv.fmt & LVCFMT_LEFT)
			nStyle = DT_LEFT;
		else if (lv.fmt & LVCFMT_CENTER)
			nStyle = DT_CENTER;
		else if (lv.fmt & LVCFMT_RIGHT)
			nStyle = DT_RIGHT;
		pDC->DrawText(str, &rect, nStyle);

		*pResult = CDRF_SKIPDEFAULT;
	}
}
