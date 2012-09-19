#include "stdafx.h"
#include "I8OldButton.h"
#include "UI/I8SkinCtrl/I8SkinCommonFunction.h"

using namespace I8SkinCtrl_ns;

IMPLEMENT_DYNAMIC(CI8OldButton, CButton)

BEGIN_MESSAGE_MAP(CI8OldButton, CButton)
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEHOVER()
    ON_WM_MOUSELEAVE()	
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()

#define TIME_EVENT_DRAWFADE 10000

CI8OldButton::CI8OldButton()
{
	m_Status = BEX_NORMAL;
	m_bType  = BTYPE_NORMAL;
	m_bgBack = m_bgHover = m_bgPressed = m_bgActive = RGB(0, 0, 0);
	m_pbkImage = m_pbkImage2 = m_pbkImage3 = NULL;
	m_phoverImage = m_phoverImage2 = m_phoverImage3 = NULL;
	m_pselectImage = m_pselectImage2 = m_pselectImage3 = NULL;
	m_pActiveImage = m_piconImage = m_piconSelImage = NULL;
	m_hIconNormal  = NULL;
	m_pdisableImage = NULL;

	m_szFont = DEFAULT_FONT_NAME;
	m_nFontSize = 12;
	m_clrText = RGB(0, 0, 0);
	m_clrHover = RGB(0, 0, 0);
	m_clrSel   = RGB(0, 0, 0);
	m_clrActive = RGB(0, 0, 0);
	m_nBTStyle = BTEXT_LEFT;
	m_bBold = m_bUnderline = m_bItalic = m_bStrictOut = FALSE;
	m_bSelBold = FALSE;
	m_bActive = FALSE;
	m_hCursor = NULL;
	m_pSourceBitmap = NULL;
	m_bAlphaMode = FALSE;
	m_DrawStatus= BDRAW_NORMAL;

	m_bf.BlendOp = AC_SRC_OVER;
	m_bf.BlendFlags = 0;
	m_bf.AlphaFormat = AC_SRC_ALPHA;
	m_bf.SourceConstantAlpha = 0;
}

CI8OldButton::~CI8OldButton()
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

void  CI8OldButton::SetImage(DWORD backImage, DWORD hoverImage, DWORD selectImage)
{
	m_pbkImage2		= I8_GetSkinImage(backImage);
	m_phoverImage2	= I8_GetSkinImage(hoverImage);
	m_pselectImage2	= I8_GetSkinImage(selectImage);
}

void CI8OldButton::SetImage(LPCTSTR backImage /* = NULL */, LPCTSTR hoverImage /* = NULL */, 
						 LPCTSTR selectImage /* = NULL */, LPCTSTR activeImage /* = NULL */,
						 LPCTSTR disableImage /*= NULL*/, BOOL bAlphaMode/* = FALSE*/)
{
	m_pbkImage2		= I8_GetSkinImage(backImage);
	m_phoverImage2	= I8_GetSkinImage(hoverImage);
	m_pselectImage2	= I8_GetSkinImage(selectImage);
	m_pActiveImage  = I8_GetSkinImage(activeImage);
	m_pdisableImage = I8_GetSkinImage(disableImage);
	m_bAlphaMode    = bAlphaMode;
}

void CI8OldButton::SetImage2(LPCTSTR backImage, LPCTSTR backImage2, LPCTSTR backImage3, 
						  LPCTSTR selectImage, LPCTSTR selectImage2, LPCTSTR selectImage3)
{
	m_pbkImage		= I8_GetSkinImage(backImage);
	m_pbkImage2		= I8_GetSkinImage(backImage2);
	m_pbkImage3		= I8_GetSkinImage(backImage3);
	m_pselectImage	= I8_GetSkinImage(selectImage);
	m_pselectImage2	= I8_GetSkinImage(selectImage2);
	m_pselectImage3	= I8_GetSkinImage(selectImage3);
}

void  CI8OldButton::SetImage3(COLORREF backColor, COLORREF hoverColor, COLORREF selectColor, COLORREF activeColor)
{
	m_bgBack	= backColor;
	m_bgHover	= hoverColor;
	m_bgPressed	= selectColor;
	m_bgActive	= activeColor;
}

void CI8OldButton::SetIconImage(LPCTSTR iconImage, LPCTSTR iconSelImage)
{
	m_piconImage	= I8_GetSkinImage(iconImage);
	m_piconSelImage = I8_GetSkinImage(iconSelImage);
}

void CI8OldButton::SetIconImage(HICON hIcon, HICON hSelIcon)
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

	m_piconImage = I8_GetSkinImage(hIcon);
	m_piconSelImage = I8_GetSkinImage(hSelIcon);

	//快速启动图标特殊处理
	if (m_bType == BTYPE_QUIKRUN)
	{
		m_hIconNormal = hIcon;
	}

	if (m_hWnd != NULL)
	{
		Invalidate();
	}
}

void  CI8OldButton::SetTextStyle(CString font, int nSize, COLORREF clrText, COLORREF clrHover, COLORREF clrSel, 
	COLORREF clrActive, BTEXT_STYLE nStyle, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bStrikeOut, BTYPE bType, BOOL bSelBold)
{
	m_szFont = font;
	m_nFontSize = nSize;
	m_clrText = clrText;
	m_clrHover = clrHover;
	m_nBTStyle = nStyle;
	m_bBold = bBold;
	m_bUnderline = bUnderline;
	m_bItalic = bItalic;
	m_bStrictOut = bStrikeOut;
	m_clrSel = clrSel;
	m_clrActive = clrActive;
	m_bSelBold = bSelBold;
	m_bType = bType;
	if (m_hWnd != NULL)
		Invalidate();
}

void CI8OldButton::SetActive(BOOL bActive)
{
	BOOL bOld = m_bActive;
	m_bActive = bActive; 
	if (bOld != m_bActive)
		Invalidate();
}

void CI8OldButton::PreSubclassWindow()
{
 	ModifyStyle(0, BS_OWNERDRAW);
	long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	CButton::PreSubclassWindow();
}

BOOL CI8OldButton::PreTranslateMessage(MSG* pMsg)
{
	if (m_toolTip.GetSafeHwnd())
	{
		m_toolTip.RelayEvent(pMsg);
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CI8OldButton::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rcClient;//(lpDis->rcItem);
	GetClientRect(&rcClient);
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
		if (pBitmap == NULL || pBitmap3 == NULL)	//只有一张图片的情况(使用第２张图片来画),这种情况可带渐隐效果
		{
			if (m_bAlphaMode)
			{				
				m_pSourceBitmap = pBitmap2;
				if (m_DrawStatus == BDRAW_NORMAL)
				{
	              I8_DrawImage(pDC, pBitmap2, rcClient);
				}				
			}
			else
			{
	           I8_DrawImage(pDC, pBitmap2, rcClient);			
			}
		}
		else	//有三张图片的情况。第１，第３不需要拉申，第２张可能需要拉伸
		{
			BITMAP bmp1;
			pBitmap->GetBitmap(&bmp1);
			I8_DrawImage(pDC, pBitmap, CRect(0, 0, bmp1.bmWidth, bmp1.bmHeight));
			
			BITMAP bmp3;
			pBitmap3->GetBitmap(&bmp3);
			CRect rc = CRect(rcClient.right - bmp3.bmWidth, 0, rcClient.right, bmp3.bmHeight);
			I8_DrawImage(pDC, pBitmap3, rc);
			
			rc = CRect(bmp1.bmWidth, 0, rcClient.right-bmp3.bmWidth, rcClient.bottom);
			I8_DrawImage(pDC, pBitmap2, rc);
		}
	}
	//绘icon.
	INT cx = 0;
	{
		if (m_bType != BTYPE_QUIKRUN)
		{	
			pBitmap = GetActive() ? m_piconSelImage : m_piconImage;
			if (pBitmap == NULL)
				pBitmap = m_piconImage;
			if (pBitmap != NULL)
			{
				CRect rx;
				GetClientRect(&rx);
				int nTop = (rx.Height() - 16)/2;
				I8_DrawImage(pDC, pBitmap, CRect(4, nTop, 20, nTop+16));
				cx = 4 + 16;
			}
		}
		else
		{
			if (m_hIconNormal != NULL)
			{	
				CRect rx;
		     	GetClientRect(&rx);
		    	int nTop = (rx.Height() - 16)/2;
				::DrawIconEx(pDC->m_hDC, 4, nTop, m_hIconNormal, 16, 16, 0, NULL, DI_NORMAL);
			}
		}
	}
	//绘文本
	CString szText;
	GetWindowText(szText);
	if (szText.IsEmpty())
	{
		return;
	}

	if (m_bType == BTYPE_QUIKRUN)
	{
		if (m_toolTip.GetSafeHwnd() == NULL)
		{
	    	m_toolTip.Create(GetParent(), TTS_ALWAYSTIP);
	    	m_toolTip.AddTool(this, szText, &rcClient, 1);		
		    m_toolTip.Activate(TRUE);
		    m_strTooltip = szText;
		}
		return;
	}

	if (m_bType != BTYPE_NBTITLE)
	{
		CFont font;
		LOGFONT lf = {0};
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
		lstrcpy(lf.lfFaceName, m_szFont);
		lf.lfHeight= m_nFontSize;
		lf.lfWeight = m_bBold ? FW_BOLD : FW_NORMAL;
		if (m_bActive && m_bSelBold)
		{
			lf.lfWeight = FW_BOLD;
		}
		lf.lfUnderline = m_bUnderline ? 1 : 0;
		lf.lfItalic = m_bItalic ? 1 : 0;
		lf.lfStrikeOut = m_bStrictOut ? 1: 0;
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
		int nStyle = 0;

		if (m_bType != BTYPE_NBCONTENT)
		{
			if (szText.FindOneOf(TEXT("\r\n")) != -1 || rect.Width() <= pDC->GetTextExtent(szText).cx)
			{
				CRect rx = rect;
				pDC->DrawText(szText, &rx, DT_CALCRECT|DT_WORDBREAK);
				bSingleLine = FALSE;
				int nHeight = rx.Height();
				rect.top += (rect.Height() - nHeight)/2;
				rect.bottom = rect.top + nHeight;
			}

			nStyle = bSingleLine ? (DT_VCENTER|DT_SINGLELINE) : (DT_VCENTER|DT_WORDBREAK);

			if (m_nBTStyle == BTEXT_LEFT)
				nStyle |= DT_LEFT;
			else if (m_nBTStyle == BTEXT_CENTER)
				nStyle |= DT_CENTER;
			else if (m_nBTStyle == BTEXT_RIGHT)
				nStyle |= DT_RIGHT;
		}
		else
		{
			nStyle = DT_TOP|DT_WORDBREAK;
		}


		pDC->DrawText(szText, -1, &rect, nStyle);
		font.DeleteObject();
	}
	else
	{
		CFont fontNormal, fontFirstLine;
		LOGFONT lf = {0};
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
		lstrcpy(lf.lfFaceName, m_szFont);
		lf.lfHeight= m_nFontSize;
		lf.lfWeight = m_bBold ? FW_BOLD : FW_NORMAL;
		lf.lfUnderline = m_bUnderline ? 1 : 0;
		lf.lfItalic = m_bItalic ? 1 : 0;
		lf.lfStrikeOut = m_bStrictOut ? 1: 0;
		fontNormal.CreateFontIndirect(&lf);

		lf.lfHeight += 2;
		lf.lfWeight = FW_BOLD;
		fontFirstLine.CreateFontIndirect(&lf);

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
		pDC->SelectObject(&fontFirstLine);

		//加32避免遮住LOGO
		CRect rect(cx + 32, rcClient.top, rcClient.right - 20, rcClient.bottom);
		CString strNbName, strNbAddr, strNbTel;
		AfxExtractSubString(strNbName, szText, 0, TEXT('|'));
		AfxExtractSubString(strNbAddr,  szText, 1, TEXT('|'));
		AfxExtractSubString(strNbTel, szText, 2, TEXT('|'));

		CRect rx = rect;	
		int   nFirstHeight = 0, nNormalHeight = 0;
		pDC->DrawText(strNbName, &rx, DT_CALCRECT);
		nFirstHeight = rx.Height();		
		rx.top = rx.bottom;
		pDC->SelectObject(&fontNormal);
		pDC->DrawText(strNbAddr, &rx, DT_CALCRECT);
		if (rx.Height() == 0 || rx.Width() == 0)
		{
			pDC->DrawText(strNbTel, &rx, DT_CALCRECT);
		}
		nNormalHeight = rx.Height();
		if (rx.Width() > rect.Width())  //地址超长
		{
			if (m_toolTip.GetSafeHwnd() == NULL)
			{
				m_toolTip.Create(GetParent(), TTS_ALWAYSTIP);
				m_toolTip.AddTool(this, strNbAddr, &rect, 1);		
				m_toolTip.Activate(TRUE);
				m_strTooltip = strNbAddr;
			}
		}
		int   nSpaceY1 = 5, nSpaceY2 = 2;
		int   nHeight = nFirstHeight + 2*nNormalHeight + nSpaceY1 + nSpaceY2;

	//	rect.top += (rect.Height() - nHeight)/2;
		rect.top += (rect.Height() -24 - nFirstHeight)/2; //临时修改，首行对齐LOGO
		rect.bottom = rect.top + nHeight;

		int nStyle = DT_VCENTER|DT_SINGLELINE;

		if (m_nBTStyle == BTEXT_LEFT)
			nStyle |= DT_LEFT;
		else if (m_nBTStyle == BTEXT_CENTER)
			nStyle |= DT_CENTER;
		else if (m_nBTStyle == BTEXT_RIGHT)
			nStyle |= DT_RIGHT;

		pDC->SelectObject(&fontFirstLine);
		rx = rect;
		rx.bottom = rx.top + nFirstHeight;
		pDC->DrawText(strNbName, -1, &rx, nStyle);

		pDC->SelectObject(&fontNormal);
		rx.top = rx.bottom + nSpaceY1;
		rx.bottom = rx.top + nNormalHeight;
		pDC->DrawText(strNbAddr, -1, &rx, nStyle|DT_WORD_ELLIPSIS);

		rx.top = rx.bottom + nSpaceY2;
		rx.bottom = rx.top + nNormalHeight;
		pDC->DrawText(strNbTel, -1, &rx, nStyle);
	}
}

void CI8OldButton::OnMouseMove(UINT nFlags, CPoint point)
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
		if (m_bAlphaMode)
		{	
			m_DrawStatus = BDRAW_ALPHA;
			m_bf.SourceConstantAlpha = 0; 
	    	SetTimer(TIME_EVENT_DRAWFADE, 20, NULL);
		}
		Invalidate();
	}
}

void CI8OldButton::OnMouseHover(UINT nFlags, CPoint point)
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

void CI8OldButton::OnMouseLeave()
{
	CButton::OnMouseLeave();

	//TRACE(TEXT("mouse leave.\r\n"));
	m_Status = BEX_NORMAL;
	if (m_bAlphaMode)
	{	
		m_DrawStatus = BDRAW_ALPHA;
		m_bf.SourceConstantAlpha = 0; 
		SetTimer(TIME_EVENT_DRAWFADE, 20, NULL);
	}
	if(m_toolTip.GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(&rect);
		m_toolTip.DelTool(this, 1);
		m_toolTip.AddTool(this, m_strTooltip, &rect, 1);		
		m_toolTip.Activate(TRUE);
	}
	Invalidate();
}

void CI8OldButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pselectImage2 != NULL)
	{
		m_Status = BEX_PRESSED;
		Invalidate();
	}
	CButton::OnLButtonDown(nFlags, point);
}

void CI8OldButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pselectImage2 != NULL)
	{
		m_Status = BEX_NORMAL;
		Invalidate();
	}

	CButton::OnLButtonUp(nFlags, point);
}

BOOL CI8OldButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CI8OldButton::OnEraseBkgnd(CDC* pDC)
{
	//CButton::OnEraseBkgnd(pDC);
	return TRUE;
}

void CI8OldButton::OnTimer( UINT_PTR nIDEvent )
{
	if (TIME_EVENT_DRAWFADE == nIDEvent 
		&& m_bf.SourceConstantAlpha <= 255)
	{
		DrawFadeEffect();
	}
	CButton::OnTimer(nIDEvent);
}

void CI8OldButton::DrawFadeEffect()
{
	HDC hDC = ::GetDC(GetSafeHwnd());
	CRect rtClient;
	GetClientRect(&rtClient);	

	CDC dc;
	dc.Attach(CreateCompatibleDC(hDC));
	CBitmap* pOldBitmap = (CBitmap*)dc.SelectObject(m_pSourceBitmap);

	// 核心部分
	m_bf.SourceConstantAlpha += 10;
	m_bf.SourceConstantAlpha = m_bf.SourceConstantAlpha >= 60 ? 255 : m_bf.SourceConstantAlpha;
	AlphaBlend(hDC, 0, 0, rtClient.Width(), rtClient.Height(), dc.m_hDC, 0, 0, rtClient.Width(), rtClient.Height(), m_bf);

	dc.SelectObject(pOldBitmap);
	dc.DeleteDC();

	::ReleaseDC(GetSafeHwnd(), hDC);	

	if (m_bf.SourceConstantAlpha >= 255)
	{
		KillTimer(TIME_EVENT_DRAWFADE);
		m_DrawStatus = BDRAW_NORMAL;
	}
}
