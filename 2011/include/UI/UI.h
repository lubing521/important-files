#pragma once

#include <afxwin.h>
#include <afxctl.h>
#include <afxhtml.h>
#include <atlimage.h>
#include <functional>
#include <memory>

namespace UI {

//tool function.
ATL::CImage* AfxUIGetImage(LPCTSTR FileName);
ATL::CImage *AfxUIGetImage(HBITMAP bmp);
void AfxUIDrawImage(CDC* pDC, const CRect& rc, ATL::CImage* pImage, ATL::CImage* pImageLeft = NULL, ATL::CImage* pImageRight = NULL);
void AfxUIDrawImage(CDC* pDC, const CRect& rc, LPCTSTR szImage, LPCTSTR szImageLeft = NULL, LPCTSTR szImageRight = NULL);
void AfxUIDrawImage(CDC* pDC, const CRect& rc, HBITMAP szImage, HBITMAP szImageLeft = NULL, HBITMAP szImageRight = NULL);

void AfxUIDrawFrameBkground(CDC* pDC, CRect& rcClient);

//conctrol class.
class CUIButton;
class CUIProgress;
class CUIHtmlCtrl;
class CUINavTab;


class CMemDC : public CDC
{
private:
	CBitmap  m_bitmap;
	CBitmap* m_oldBitmap;
	CDC*	 m_pDC;
	CRect    m_rect;
	BOOL     m_bMemDC;
public:
	CMemDC(CDC* pDC, const CRect* pRect = NULL) : CDC()
	{
		ASSERT(pDC != NULL);
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();
		if (pRect == NULL)
			pDC->GetClipBox(&m_rect);
		else
			m_rect = *pRect;
		if (m_bMemDC)
		{
			CreateCompatibleDC(pDC);
			pDC->LPtoDP(&m_rect);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);
			SetMapMode(pDC->GetMapMode());
			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());
			pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
		FillSolidRect(m_rect, pDC->GetBkColor());
	}

	~CMemDC()         
	{
		if (m_bMemDC)
		{
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, SRCCOPY);
			SelectObject(m_oldBitmap);
		}
		else
		{
			m_hDC = m_hAttribDC = NULL;   
		}
	}
	CMemDC* operator->()
	{
		return this;
	}
	operator CMemDC*()
	{
		return this;
	}
};

typedef enum BEX_STATUS
{
	BEX_NORMAL	= 0,
	BEX_PRESSED	= 1,
	BEX_HOVER	= 2
}BEXSTATUS;

class CUIButton : public CButton
{
	DECLARE_DYNAMIC(CUIButton)
public:
	CUIButton();
	virtual ~CUIButton();
public:
	inline BEX_STATUS GetStatus()	{ return m_Status;	}
	inline BOOL IsPressed() { return (m_Status == BEX_PRESSED); }
	inline BOOL IsHovered() { return (m_Status == BEX_HOVER);   }

	//set image.
	void SetImage(LPCTSTR normalImage, LPCTSTR hoverImage = NULL, LPCTSTR pressImage = NULL, LPCTSTR disableImage = NULL);
	void SetImage(HBITMAP normal, HBITMAP hover = NULL, HBITMAP press = NULL, HBITMAP disable = NULL);

	void SetImage2(LPCTSTR normalImage, LPCTSTR normalImageLeft, LPCTSTR normalImageRight,
		LPCTSTR hoverImage, LPCTSTR hoverImageLeft, LPCTSTR hoverImageRight,
		LPCTSTR pressImage, LPCTSTR pressImageLeft, LPCTSTR pressImageRight,
		LPCTSTR disableImage, LPCTSTR disableImageLeft, LPCTSTR disableImageRight);

	//set left icon.
	void SetIcon(LPCTSTR noralIconImage, LPCTSTR hoverIconImage, LPCTSTR pressIconImage, LPCTSTR disableIconImage, 
		long nIconLeft = -1, long nIconTop = -1);

	//set caption text.
	void SetTextStyle(COLORREF clrBack, COLORREF clrNoral, COLORREF clrHover, COLORREF clrPress, COLORREF clrDisable,
		CFont* pFont = NULL, long nLeftAlign = -1);
	
	// Fit2Size()
	void Fit2Size();

protected:
	void FreeImageResource(bool includenormal = false, bool includehover = false, bool includepress = false, 
		bool includedisable = false, bool includeicon = false);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	BEXSTATUS m_Status;
	ATL::CImage *m_pNormalImage,  *m_pNormalImageLeft,  *m_pNormalImageRigth;
	ATL::CImage *m_pHoverImage,   *m_pHoverImageLeft,	*m_pHoverImageRigth;
	ATL::CImage *m_pPressImage,   *m_pPressImageLeft,	*m_pPressImageRigth;
	ATL::CImage *m_pDisableImage, *m_pDisableImageLeft, *m_pDisableImageRigth;
	ATL::CImage *m_pNormalIconImage, *m_pHoverIconImage, *m_pPressIconImage, *m_pDisableIconImage;
	long		 m_nIconLeft, m_nIconTop;
	
	COLORREF	 m_clrBackground;
	COLORREF	 m_clrCapNormal;
	COLORREF	 m_clrCapHover;
	COLORREF	 m_clrCapPress;
	COLORREF     m_clrCapDisable;
	CFont		 m_fCapFont;
	long		 m_nTextLeft;
};

class CUIProgress : public CButton
{
	DECLARE_DYNAMIC(CUIProgress)
public:
	CUIProgress();
	virtual ~CUIProgress();
public:
	void  SetHorzScroll(BOOL bHorz = TRUE);
	BOOL  IsHorzScroll()	{ return m_bIsHorz; }
	void  SetProgres(float fProgress /* 0.0 - 100.0 */);
	float GetProgress()	{ return m_fProgress; }
	void  SetImage(LPCTSTR lpszBkImage = NULL, LPCTSTR lpszFgImage = NULL);
protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP();
private:
	BOOL  m_bIsHorz;
	float m_fProgress;
	ATL::CImage* m_pbkImage, *m_pfgImage;
};



class CUIHtmlCtrl : public CHtmlView
{
	DECLARE_DYNAMIC(CUIHtmlCtrl)
public:
	CUIHtmlCtrl();
	virtual ~CUIHtmlCtrl();
public:
	void SetScroll(BOOL bHaveScroll);
private:
	BOOL m_bScroll;
	virtual void	OnTitleChange(LPCTSTR lpszText);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *info);
	virtual void	OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
		LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);	

	virtual void PostNcDestroy() {  }
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP();
};

#define NAV_ITEM_COUNT		10
#define NAV_ITEM_WITDH		107
#define NAV_ITEM_HEIGHT		28

class CUINavTab : public CButton
{
	DECLARE_DYNAMIC(CUINavTab)
public:
	typedef std::tr1::function<CWnd *()> EventCallback;

	CUINavTab();
	virtual ~CUINavTab();

	void Register(size_t index, const EventCallback &callback);

	template <typename StringT, size_t N>
	void SetImages(const StringT (&images)[N], LPCTSTR menuBgLeft, LPCTSTR menuBgMid, LPCTSTR menuBgRight)
	{
		m_pBkImageLeft	= AfxUIGetImage(menuBgLeft);
		m_pBkImageCenter= AfxUIGetImage(menuBgMid);
		m_pBkImageRight = AfxUIGetImage(menuBgRight);

		for (long idx=0; idx < N; idx++)
		{
			m_pNavImages[idx] = AfxUIGetImage(images[idx].c_str());
		}
		m_nActive = 1; //un active.= -1
	}

	template <size_t N>
	void SetImages(const HBITMAP (&images)[N], HBITMAP menuBgLeft, HBITMAP menuBgMid, HBITMAP menuBgRight)
	{
		m_pBkImageLeft = AfxUIGetImage(menuBgLeft);
		m_pBkImageCenter = AfxUIGetImage(menuBgMid);
		m_pBkImageRight = AfxUIGetImage(menuBgRight);

		for (long idx=0; idx < N; idx++)
		{
			m_pNavImages[idx] = AfxUIGetImage(images[idx]);
		}
		m_nActive = 1; //un active.= -1
	}

	void Select(size_t pos);

protected:
	long GetActiveByMouse(CPoint pt);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
private:
	ATL::CImage* m_pBkImageLeft, *m_pBkImageCenter, *m_pBkImageRight;
	ATL::CImage* m_pNavImages[NAV_ITEM_COUNT * 2];
	BEXSTATUS    m_Status;
	long		 m_nActive;
};

}