// ButtonEx.cpp : 实现文件
//
#include "stdafx.h"
#include "ui.h"
#include "MsgDlg.h"

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

#ifndef __i8desk_locker__
void AfxMessageBoxEx(LPCTSTR lpszText)
{
	CMsgDlg dlg;
	dlg.m_strContent = lpszText;
	dlg.DoModal();
}
#endif

#define   WIDTHBYTES(bits)       ((((bits) + 31)>>5)<<2) 
CIcon::CIcon() 
{ 
	_hIcon = NULL; 
	_lPos  = 0; 
} 

CIcon::~CIcon() 
{ 
} 

UINT   CIcon::CheckHead(const LPBYTE pBuff, long lSize) 
{ 
	_lPos = 0; 
	WORD   input; 

	if(lSize <= sizeof(WORD)*3) 	return   (UINT)-1; 
	memcpy(&input, pBuff + _lPos, sizeof(WORD));
	_lPos += sizeof(WORD); 

	if(input != 0) 	return -1; 
	memcpy(&input, pBuff + _lPos, sizeof(WORD));
	_lPos += sizeof(WORD); 

	if(input != 1) return -1; 
	memcpy(&input, pBuff + _lPos,sizeof(WORD));
	_lPos += sizeof(WORD); 

	return   input;   //pBuff中包含的图标个数 
} 

LPSTR   CIcon::FindDIBBits(LPSTR lpbi) 
{ 
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi)); 
} 

DWORD   CIcon::PaletteSize(LPSTR lpbi) 
{ 
	return (DIBNumColors(lpbi)*sizeof(RGBQUAD)); 
} 

DWORD   CIcon::DIBNumColors(LPSTR lpbi) 
{ 
	WORD    wBitCount; 
	DWORD   dwClrUsed; 

	dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)-> biClrUsed; 
	if   (dwClrUsed)  return (WORD)dwClrUsed; 
	wBitCount = ((LPBITMAPINFOHEADER)lpbi)-> biBitCount; 

	switch(wBitCount)   
	{ 
	case   1: return   2; 
	case   4: return   16; 
	case   8: return   256; 
	case   16:return   65535; 
	case   32:return   4294967295; 
	default:  return   0; 
	} 
	return   0; 
} 

DWORD   CIcon::BytesPerLine(LPBITMAPINFOHEADER   lpBMIH) 
{ 
	DWORD dwBits = lpBMIH->biWidth*lpBMIH->biPlanes*lpBMIH->biBitCount;
	return WIDTHBYTES(dwBits); 
} 


HICON   CIcon::MakeIconFromResource(LPICONIMAGE   lpIcon) 
{ 	
	if(lpIcon == NULL || lpIcon->lpBits == NULL) return NULL; 

	HICON hIcon = CreateIconFromResourceEx(lpIcon-> lpBits, lpIcon-> dwNumBytes, TRUE, 0x00030000,   
		(*(LPBITMAPINFOHEADER)(lpIcon-> lpBits)).biWidth, (*(LPBITMAPINFOHEADER)(lpIcon-> lpBits)).biHeight/2, 0); 

	if(hIcon == NULL && lpIcon->lpbi->bmiHeader.biBitCount != 16) 
	{ 
		hIcon = CreateIconFromResource(lpIcon->lpBits, lpIcon->dwNumBytes, TRUE, 0x00030000); 
	} 

	return hIcon; 
} 

BOOL   CIcon::CreateFromBuff(LPBYTE   pBuff,long   lDataSize) 
{ 
	if(pBuff == NULL || lDataSize == 0) 
	{ 
		return  FALSE; 
	} 
	if (CheckHead(pBuff,lDataSize) == (UINT)-1) return   FALSE; 
	LPICONDIRENTRY lpIDE = (LPICONDIRENTRY)malloc(sizeof(ICONDIRENTRY)); 

	if (lpIDE == NULL)  return FALSE; 
	memcpy(lpIDE, pBuff + _lPos, sizeof(ICONDIRENTRY)); 

	LPICONIMAGE lpIconImage = (LPICONIMAGE)malloc(sizeof(ICONIMAGE)); 
	if   (lpIconImage == NULL) return FALSE; 

	lpIconImage->lpBits = (LPBYTE)malloc(lpIDE->dwBytesInRes); 
	memcpy(lpIconImage->lpBits, pBuff + lpIDE->dwImageOffset,lpIDE->dwBytesInRes); 
	lpIconImage->lpbi   = (LPBITMAPINFO)lpIconImage->lpBits; 
	lpIconImage->Width  = lpIconImage->lpbi->bmiHeader.biWidth; 
	lpIconImage->Height = (lpIconImage->lpbi->bmiHeader.biHeight)/2; 
	lpIconImage->Colors = lpIconImage->lpbi->bmiHeader.biPlanes*lpIconImage->lpbi->bmiHeader.biBitCount; 
	lpIconImage->lpXOR  = (LPBYTE)FindDIBBits((LPSTR)lpIconImage-> lpbi); 
	lpIconImage->lpAND  = lpIconImage->lpXOR + (lpIconImage->Height*BytesPerLine((LPBITMAPINFOHEADER)lpIconImage->lpbi)); 

	HICON hTemp = MakeIconFromResource(lpIconImage); 
	if(hTemp != NULL) 
	{ 
		if (_hIcon != NULL) 
		{ 
			DestroyIcon(_hIcon); 
		} 
		_hIcon = hTemp; 
	} 

	free(lpIconImage->lpBits); 
	free(lpIconImage); 
	free(lpIDE); 

	return TRUE; 
} 

IMPLEMENT_DYNAMIC(CButtonEx, CButton)

CButtonEx::CButtonEx()
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
	ON_WM_TIMER()
END_MESSAGE_MAP()

void  CButtonEx::SetImage(DWORD backImage, DWORD hoverImage, DWORD selectImage)
{
	m_pbkImage2		= AfxGetSkinImage(backImage);
	m_phoverImage2	= AfxGetSkinImage(hoverImage);
	m_pselectImage2	= AfxGetSkinImage(selectImage);
}

void CButtonEx::SetImage(LPCTSTR backImage /* = NULL */, LPCTSTR hoverImage /* = NULL */, 
						 LPCTSTR selectImage /* = NULL */, LPCTSTR activeImage /* = NULL */,
						 LPCTSTR disableImage /*= NULL*/, BOOL bAlphaMode/* = FALSE*/)
{
	m_pbkImage2		= AfxGetSkinImage(backImage);
	m_phoverImage2	= AfxGetSkinImage(hoverImage);
	m_pselectImage2	= AfxGetSkinImage(selectImage);
	m_pActiveImage  = AfxGetSkinImage(activeImage);
	m_pdisableImage = AfxGetSkinImage(disableImage);
	m_bAlphaMode    = bAlphaMode;
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

void  CButtonEx::SetTextStyle(CString font, int nSize, COLORREF clrText, COLORREF clrHover, COLORREF clrSel, 
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

BOOL CButtonEx::PreTranslateMessage(MSG* pMsg)
{
	if (m_toolTip.GetSafeHwnd())
	{
		m_toolTip.RelayEvent(pMsg);
	}
	return CButton::PreTranslateMessage(pMsg);
}

void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDis)
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
	              AfxDrawImage(pDC, pBitmap2, rcClient);
				}				
			}
			else
			{
	           AfxDrawImage(pDC, pBitmap2, rcClient);			
			}
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
				AfxDrawImage(pDC, pBitmap, CRect(4, nTop, 20, nTop+16));
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
		if (m_bAlphaMode)
		{	
			m_DrawStatus = BDRAW_ALPHA;
			m_bf.SourceConstantAlpha = 0; 
	    	SetTimer(TIME_EVENT_DRAWFADE, 20, NULL);
		}
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

void CButtonEx::OnTimer( UINT_PTR nIDEvent )
{
	if (TIME_EVENT_DRAWFADE == nIDEvent 
		&& m_bf.SourceConstantAlpha <= 255)
	{
		DrawFadeEffect();
	}
	CButton::OnTimer(nIDEvent);
}

void CButtonEx::DrawFadeEffect()
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
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage(WM_HTTP_ERROR_MESSAGE);
	}
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
	m_pHeadRightImage	= AfxGetSkinImage(TEXT("Skin/Dock/head_right.png"));
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
	if (m_pHeadRightImage)
	{
		m_pHeadRightImage->DeleteObject();
		delete m_pHeadRightImage;
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
		for (int idx=0; idx<_countof(m_btnTool)&&idx!=3; idx++)
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

		//rx = rc;
		//rx.left = rc.right - 241;
		//rx.bottom = 27;
		//AfxDrawImage(pDC, m_pbkTipImage, rx);

		rx = rc;
		rx.left = rx.right - 260;
		rx.bottom = rc.bottom;
		AfxDrawImage(pDC, m_pHeadRightImage, rx);

		//6个导航栏按钮两边的拉伸(6个按钮的宽度(82+2)*6-2=502)
		rx = rc;
		rx.right = rc.right - 260;
		rx.left = (rc.Width() + 502)/2;
		AfxDrawImage(pDC,m_pbkImage, rx);

		rx.left = 65+206;
		rx.right = (rc.Width() - 502)/2;
		AfxDrawImage(pDC, m_pbkImage, rx);
	}
	else
	{
		//CRect rx = rc;
		//rx.right = 7;
		//AfxDrawImage(pDC, m_pbkImage, rx);

		//rx = rc;
		//rx.left = rc.right - 7;
		//AfxDrawImage(pDC, m_pbkImage, rx);
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
	for (int idx=0; idx<6; idx++)
	{
		m_btnNav[2*idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 
			DOCK_NAVBAR_INDEX + idx);
		m_btnNav[2*idx+1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 0xffff);
		m_btnNav[2*idx+1].SetImage(TEXT("Skin/Dock/背景_头部_按钮分割线.png"));
		m_btnNav[2*idx].SetCursor(hCursor);
	}
	m_btnNav[0].SetImage(TEXT("Skin/Dock/按钮_浏览器_默认状态.png"), TEXT("Skin/Dock/按钮_浏览器_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_浏览器_鼠标按下.png"), TEXT("Skin/Dock/按钮_浏览器_激活.png"), NULL, TRUE);
	m_btnNav[2].SetImage(TEXT("Skin/Dock/按钮_游戏大厅_默认状态.png"), TEXT("Skin/Dock/按钮_游戏大厅_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_游戏大厅_鼠标按下.png"), TEXT("Skin/Dock/按钮_游戏大厅_激活.png"), NULL, TRUE);
	m_btnNav[4].SetImage(TEXT("Skin/Dock/按钮_聊天工具_默认状态.png"), TEXT("Skin/Dock/按钮_聊天工具_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_聊天工具_鼠标按下.png"), TEXT("Skin/Dock/按钮_聊天工具_激活.png"), NULL, TRUE);
	m_btnNav[6].SetImage(TEXT("Skin/Dock/按钮_常用软件_默认状态.png"), TEXT("Skin/Dock/按钮_常用软件_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_常用软件_鼠标按下.png"), TEXT("Skin/Dock/按钮_常用软件_激活.png"), NULL, TRUE);
	m_btnNav[8].SetImage(TEXT("Skin/Dock/按钮_点卡商城_默认状态.png"), TEXT("Skin/Dock/按钮_点卡商城_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_点卡商城_鼠标按下.png"), TEXT("Skin/Dock/按钮_点卡商城_激活.png"), NULL, TRUE);
	m_btnNav[10].SetImage(TEXT("Skin/Dock/按钮_影视音乐_默认状态.png"), TEXT("Skin/Dock/按钮_影视音乐_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_影视音乐_鼠标按下.png"), TEXT("Skin/Dock/按钮_影视音乐_激活.png"), NULL, TRUE);

	DWORD dwID = DOCK_NAVBAR_INDEX + 6;
	m_btnTool[0].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[2].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[3].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[4].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[5].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnNetBar.Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnNetBar.SetImage(TEXT("Skin/Dock/网吧名称_背景_默认状态.png"), TEXT("Skin/Dock/网吧名称_背景_鼠标经过.png"), 
		TEXT("Skin/Dock/网吧名称_背景_鼠标按下.png"));
	m_btnNetBar.SetTextStyle(DEFAULT_FONT_NAME, 12, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, CButtonEx::BTEXT_LEFT, FALSE, FALSE, FALSE, FALSE, CButtonEx::BTYPE_NBTITLE);
	m_btnNetBar.SetCursor(hCursor);

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

	m_btnTool[3].ShowWindow(SW_HIDE);
	m_btnTool[3].SetCursor(hCursor);
	m_btnTool[4].SetCursor(hCursor);
	m_btnTool[5].SetCursor(hCursor);

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
	//	rc.left = 7;
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

	GetTabSkinColor();
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

void CDockTabBar::GetTabSkinColor()
{
    m_clrNormal = RGB(255, 255, 255);;
	m_clrHover  = RGB(255, 216, 0);
	m_clrSel    = RGB(0, 0, 0);

	TCHAR szSkinPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szSkinPath, MAX_PATH);
	PathRemoveFileSpec(szSkinPath);
	PathAddBackslash(szSkinPath);
	lstrcat(szSkinPath, TEXT("Skin\\"));
	lstrcat(szSkinPath, SKIN_FILE_NAME);
	BOOL bRet = CZipSkin::getInstance()->ExtractItem(TEXT("Skin/skin.ini"), szSkinPath);
	if (bRet)
	{
		TCHAR szValue[MAX_PATH] = {0}; TCHAR* pTemp = NULL; DWORD dwRet = 0;
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("NormalColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	 m_clrNormal = wcstol(szValue, &pTemp, 16);

        ZeroMemory(szValue, MAX_PATH);
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("HoverColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	 m_clrHover = wcstol(szValue, &pTemp, 16);

		ZeroMemory(szValue, MAX_PATH);
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("SelectColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	m_clrSel = wcstol(szValue, &pTemp, 16);
	}
}


CButtonEx* CDockTabBar::AddTab(LPCTSTR lpszCap, LPCTSTR iconImage, LPCTSTR iconSelImage, BOOL bRefresh /* = FALSE */)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	CButtonEx* pTab = new CButtonEx;
	pTab->Create(TEXT("Tab"), BS_CENTER|WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, DOCK_TAB_INDEX+1+m_BtnTabGroup.size());
	pTab->SetImage(TEXT("Skin/TabBar/背景_未选中标签.png"), TEXT("Skin/TabBar/背景_未选中标签.png"),
		TEXT("Skin/TabBar/背景_选中标签.png"), TEXT("Skin/TabBar/背景_选中标签.png"));
	pTab->SetWindowText(lpszCap);
	pTab->SetTextStyle(DEFAULT_FONT_NAME, 12, m_clrNormal, m_clrHover, m_clrSel, m_clrSel, CButtonEx::BTEXT_CENTER, FALSE, FALSE, FALSE, FALSE, CButtonEx::BTYPE_NORMAL, TRUE);
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

void CDockStatusBar::SetQuickRunGroup(int nIndex, HICON hIcon, CString strCaption)
{
	if (nIndex <0 || nIndex > 4)
		return ;

	m_BtnGroup[nIndex].SetIconImage(hIcon);
	m_BtnGroup[nIndex].SetWindowText(strCaption);

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
		m_BtnGroup[idx].SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
			CButtonEx::BTEXT_LEFT, FALSE, FALSE, FALSE, FALSE, CButtonEx::BTYPE_QUIKRUN);
		if (idx<5)
			m_BtnGroup[idx].SetImage(TEXT("Skin/StatusBar/背景_状态栏_16x16图标背景.png"));
	}

	m_BtnGroup[5].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_左.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_1px.png"));

	m_WebPage.SetNoScroll();
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), &m_BtnGroup[6], 0xffff);
	m_WebPage.SetSilent(TRUE);

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar10_wz.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID),  szMacAddr);
	m_WebPage.Navigate2(strUrl);

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

	m_BtnGroup[14].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_输入法_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_输入法_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_输入法_鼠标按下.png"));

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
	CRect rc(65, 5, 89, 28);
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
	rc.right = rcClient.Width() - 450 + 11;
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
	m_BtnGroup[14].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
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


CDockHtmlCtrlEx::CDockHtmlCtrlEx()
{

}

IMPLEMENT_DYNAMIC(CDockHtmlCtrlEx, CStatic)

BEGIN_MESSAGE_MAP(CDockHtmlCtrlEx, CStatic)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CDockHtmlCtrlEx::~CDockHtmlCtrlEx()
{

}

int CDockHtmlCtrlEx::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	CRect rc;
	GetClientRect(&rc);
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, rc, this, AFX_IDW_PANE_FIRST);
	m_WebPage.SetSilent(TRUE);

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar09_gg.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID),  szMacAddr);
	m_WebPage.Navigate2(strUrl);
	return 1;
}


IMPLEMENT_DYNAMIC(CStandardDialogBase, CDialog)
CStandardDialogBase::CStandardDialogBase( UINT nIDTemplate, COLORREF clrTransparent /*= RGB(0, 255, 0)*/, CWnd* pParentWnd /*= NULL*/ )
:CDialog(nIDTemplate, pParentWnd)
{
	m_clrTransparent = clrTransparent;
}

CStandardDialogBase::~CStandardDialogBase()
{

}

BEGIN_MESSAGE_MAP(CStandardDialogBase, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH CStandardDialogBase::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	if (nCtlColor == CTLCOLOR_DLG)
		return m_brush;

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CStandardDialogBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_BORDER) == 0);
	VERIFY((dwStyle & WS_SIZEBOX) == 0);
	VERIFY((dwStyle & WS_DLGFRAME) == 0);

	m_brush.CreateSolidBrush(m_clrTransparent);
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	VERIFY((dwExStyle & WS_EX_APPWINDOW) == 0);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HMODULE hInst = LoadLibrary(_T("User32.DLL")); 
	typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD); 
	MYFUNC SetLayeredWindowAttributes = NULL; 
	SetLayeredWindowAttributes = (MYFUNC)::GetProcAddress(hInst, "SetLayeredWindowAttributes"); 
	SetLayeredWindowAttributes(GetSafeHwnd(), m_clrTransparent, 0, 1); 
	::FreeLibrary(hInst);

	return TRUE;
}

void CStandardDialogBase::EndDialog( int nResult )
{
	CWnd* hParent = GetParent();
	if (hParent != NULL)
	{
		HWND hWnd = hParent->m_hWnd; 
		::SendMessage(hWnd, WM_CLOSE, 0, 0); 
	}

	CDialog::EndDialog(nResult);
}

void CStandardDialogBase::ShowDialog()
{
	Create(m_lpszTemplateName, m_pParentWnd);
	ShowWindow(SW_SHOW);
}

void CStandardDialogBase::OnOK()
{
	GetParent()->DestroyWindow();
}

void CStandardDialogBase::OnCancel()
{
	GetParent()->DestroyWindow();
}


IMPLEMENT_DYNAMIC(CTransDialogBase, CDialog)

CTransDialogBase::CTransDialogBase( UINT nIDTemplate, LPCTSTR lpszImageFile, CWnd* pParent, void* pParam )
: CDialog(nIDTemplate, pParent)
{
	m_pImage = AfxGetSkinImage(lpszImageFile);
	ASSERT(m_pImage);
	m_pStandardDialog = NULL;
	m_pStandardDialogParam = pParam;
}

CTransDialogBase::~CTransDialogBase()
{
	if (m_pStandardDialog != NULL)
	{
		delete m_pStandardDialog;
		m_pStandardDialog = NULL;
	}
	if (m_pImage != NULL)
	{
		m_pImage->DeleteObject();
		delete m_pImage;
		m_pImage = NULL;
	}
}

BEGIN_MESSAGE_MAP(CTransDialogBase, CDialog)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CTransDialogBase::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

int CTransDialogBase::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	// Must be overlapped
	VERIFY((lpCreateStruct->style & WS_POPUP) == 0);
	VERIFY((lpCreateStruct->style & WS_CHILD) == 0);

	BITMAP bm;
	m_pImage->GetBitmap(&bm);

	lpCreateStruct->x = 0;
	lpCreateStruct->y = 0;

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pStandardDialog = CreateStandardDialog(m_pStandardDialogParam);
	if (m_pStandardDialog != NULL)
	{
		m_pStandardDialog->ShowDialog();
	}

	ModifyStyle(WS_CAPTION, WS_POPUP, 0);

	MoveWindow(0, 0, bm.bmWidth, bm.bmHeight, FALSE);
	CenterWindow();

	return 0;
}

BOOL CTransDialogBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	// must be WS_POPUP
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_CHILD) == 0);

	m_bf.BlendOp = AC_SRC_OVER;
	m_bf.BlendFlags = 0;
	m_bf.AlphaFormat = AC_SRC_ALPHA;
	m_bf.SourceConstantAlpha = 255;//AC_SRC_ALPHA

	UpdateView();

	return TRUE;
}

void CTransDialogBase::OnMove( int x, int y )
{
	CDialog::OnMove(x, y);
	if (m_pStandardDialog != NULL)
	{
		::SetWindowPos(m_pStandardDialog->GetSafeHwnd(), NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
}

void CTransDialogBase::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	if (m_pStandardDialog != NULL)
	{
		::SetWindowPos(m_pStandardDialog->GetSafeHwnd(), NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CTransDialogBase::OnLButtonDown( UINT nFlags, CPoint point )
{
	::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, 0xF012, 0);
	CDialog::OnLButtonDown(nFlags, point);
}

CStandardDialogBase* CTransDialogBase::GetStandardDialog()
{
	return m_pStandardDialog;
}

void CTransDialogBase::UpdateView()
{
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED);

	CRect rtClient, rtWindow;
	GetClientRect(&rtClient);
	GetWindowRect(&rtWindow);

	CDC* pDC = GetDC();
	CDC  dcMem, dcImage;	

	dcMem.CreateCompatibleDC(pDC);
	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((rtWindow.Width() * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = rtWindow.Width();   
	stBmpInfoHeader.biHeight = rtWindow.Height();   
	stBmpInfoHeader.biPlanes = 1;
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * rtWindow.Height();    

	PUINT32 pvBits = NULL;   
	HBITMAP hBitMap = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, (LPVOID*)&pvBits, NULL, 0);
	dcMem.SelectObject(hBitMap);
	dcImage.CreateCompatibleDC(&dcMem);
	dcImage.SelectObject(m_pImage);
	dcMem.AlphaBlend(0, 0, rtClient.Width(), rtClient.Height(), &dcImage, 0, 0, rtClient.Width(), rtClient.Height(), m_bf);	

	HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
	typedef BOOL (WINAPI *DecUpdateLayeredWindow)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
	DecUpdateLayeredWindow UpdateLayeredWindow;
	UpdateLayeredWindow = (DecUpdateLayeredWindow)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");

	POINT ptDes = { rtWindow.left, rtWindow.top};
	POINT ptSrc = { 0, 0};
	SIZE  size  = { rtWindow.Width(), rtWindow.Height()};
	UpdateLayeredWindow(m_hWnd, pDC->m_hDC, &ptDes, &size, dcMem.m_hDC, &ptSrc, 0, &m_bf, 2);

	DeleteObject(hBitMap);
	dcImage.DeleteDC();
	dcMem.DeleteDC();
	ReleaseDC(pDC);
	FreeLibrary(hFuncInst);
}

void CTransDialogBase::OnOK()
{
    DestroyWindow();
}

void CTransDialogBase::OnCancel()
{
    DestroyWindow();
}