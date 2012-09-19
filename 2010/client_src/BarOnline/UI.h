#pragma once

#include <afxhtml.h>
#include <gdiplus.h>
#include <map>
#include <list>
#include <vector>
#include "XUnzip.h"

#define FRAME_CLASSNAME		TEXT("DeskXFrame")
#define FILE_MAP_NAME		TEXT("__i8desk_baronline_map__")
#define SKIN_FILE_NAME      TEXT("Skin.ini")

#define DEFAULT_FONT_NAME	TEXT("宋体")
#define DEFAULT_TIP_TEXT1	TEXT("请输入关键字符,例如传奇世界或CQSJ")
#define DEFAULT_TIP_TEXT2	TEXT("请输入网址")


static int WM_TRAY_MESSAGE			= /*WM_USER + 0x0100; //*/RegisterWindowMessage(TEXT("_i8desk_tray_message"));
static int WM_REFRESH_DATA			= RegisterWindowMessage(TEXT("_i8desk_refresh_data"));
static int WM_EXEC_START_PARAM		= RegisterWindowMessage(TEXT("_i8desk_exec_start_param"));
static int WM_NAV_CLICK_MESSAGE		= RegisterWindowMessage(TEXT("_i8desk_nav_click_message"));
static int WM_BROWSE_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_browse_click_message"));
static int WM_FINDER_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_finder_click_message"));
static int WM_TAB_CLICK_MESSAGE		= RegisterWindowMessage(TEXT("_i8desk_tab_click_message"));
static int WM_STATUS_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_status_click_message"));
static int WM_HTTP_ERROR_MESSAGE    = RegisterWindowMessage(TEXT("_i8desk_http_error_message"));

#define FRAME_DOCK_WIDTH	512
#define FRAME_DOCK_HEIGHT	58
#define FRAME_MINWIDTH		1024
#define FRAME_MINHEIGHT		738

#define TIME_EVENT_DRAWFADE 10000


//内存加载ICON类
class   CIcon     
{ 
public: 
	CIcon(); 
	virtual   ~CIcon(); 

public:
	typedef   struct   
	{ 
		UINT           Width,Height,Colors; 
		LPBYTE         lpBits; 
		DWORD          dwNumBytes; 
		LPBITMAPINFO   lpbi; 
		LPBYTE         lpXOR; 
		LPBYTE         lpAND; 
	}ICONIMAGE,*LPICONIMAGE; 
	typedef   struct   
	{ 
		BYTE           bWidth; 
		BYTE           bHeight; 
		BYTE           bColorCount; 
		BYTE           bReserved; 
		WORD           wPlanes; 
		WORD           wBitCount; 
		DWORD          dwBytesInRes; 
		DWORD          dwImageOffset; 
	}ICONDIRENTRY,*LPICONDIRENTRY; 

	BOOL    CreateFromBuff(LPBYTE pBuff,long lDataSize); 
	HICON   GetHICON(){return   _hIcon;}; 

protected: 
	UINT    CheckHead(const   LPBYTE   pBuff,long   lSize); 
	LPSTR   FindDIBBits(LPSTR   lpbi); 
	DWORD   PaletteSize(LPSTR   lpbi); 
	DWORD   DIBNumColors(LPSTR   lpbi); 
	DWORD   BytesPerLine(LPBITMAPINFOHEADER   lpBMIH); 
	HICON   MakeIconFromResource(LPICONIMAGE   lpIcon); 

private: 
	HICON   _hIcon; 
	long    _lPos; 
}; 

class CZipSkin
{
public:
	CZipSkin()
	{
	}
	~CZipSkin()
	{
		if (m_hZip != NULL)
		{
			CloseZip(m_hZip);
			m_hZip = NULL;
		}
		if (m_pZipFile != NULL)
		{
			delete []m_pZipFile;
			m_pZipFile = NULL;
		}
	}
public:
	static CZipSkin* getInstance()
	{
		static CZipSkin skin;
		return &skin;
	}
	BOOL   LoadSkinInfo(const CString& strSkinInfo)
	{
		CString strSkinFile;
		AfxExtractSubString(strSkinFile, strSkinInfo, 1, TEXT('|'));
		strSkinFile = CString(TEXT("Skin\\")) + strSkinFile;
		if(!LoadZip(strSkinFile))
		{
			if (!LoadZip(TEXT("skin\\skin.zip")))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	BOOL   LoadZip(const CString& strPath)
	{
		m_hZip = NULL;
		m_pZipFile = NULL;
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, sizeof(szFile)-1);
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		_tcscat_s(szFile, strPath);
		HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = GetFileSize(hFile, NULL);
			m_pZipFile = new char[dwSize + 1];
			DWORD dwReadBytes = 0;
			ReadFile(hFile, m_pZipFile, dwSize, &dwReadBytes, NULL);
			m_pZipFile[0] = 0;
			CloseHandle(hFile);
			m_hZip = OpenZip(m_pZipFile, dwSize, ZIP_MEMORY);
			if (m_hZip != NULL)
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	CBitmap* LoadSkinFromHIcon(HICON hIcon)
	{
		CBitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pImage = Gdiplus::Bitmap::FromHICON(hIcon);
		if (pImage != NULL)
		{
			HBITMAP  hBitmap = NULL;
			if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
			{
				pBitmap = new CBitmap;
				pBitmap->Attach(hBitmap);
			}
			delete pImage;
		}
		return pBitmap;
	}

	CBitmap* LoadSkinFromFile(LPCTSTR szFile)
	{
		CBitmap* pBitmap = NULL;
		Gdiplus::Bitmap* pImage = Gdiplus::Bitmap::FromFile(szFile);
		if (pImage != NULL)
		{
			HBITMAP hBitmap = NULL;
			if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
			{
				pBitmap = new CBitmap;
				pBitmap->Attach(hBitmap);
			}
			delete pImage;
		}
		return pBitmap;
	}

	CBitmap* LoadSkinFromResId(DWORD dwResId, LPCTSTR lpszResType = TEXT("PNG"))
	{
		HGLOBAL hGlobal = NULL;
		{
			HMODULE hMod = AfxGetInstanceHandle();
			HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(dwResId), lpszResType);
			if (hRes != NULL)
			{
				HGLOBAL hResGlobal = LoadResource(hMod, hRes);
				if (hResGlobal != NULL)
				{
					LPVOID  lpData = LockResource(hResGlobal);
					if (lpData != NULL)
					{
						DWORD dwSize = SizeofResource(hMod, hRes);
						hGlobal = GlobalAlloc(GMEM_FIXED, dwSize);
						LPVOID lpVoid = GlobalLock(hGlobal);
						if (lpVoid == NULL)
						{
							GlobalFree(hGlobal);
							hGlobal = NULL;
						}
						else
						{
							memcpy(lpVoid, lpData, dwSize);
							GlobalUnlock(hGlobal);
						}
					}
				}
			}
		}

		CBitmap* pBitmap = NULL;
		if (hGlobal != NULL)
		{
			IStream* pIStream = NULL;
			Gdiplus::Bitmap* pImage = NULL;
			DWORD dwError = CreateStreamOnHGlobal(hGlobal, TRUE, &pIStream);
			if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
			{
				HBITMAP hBitmap = NULL;
				if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
				{
					pBitmap = new CBitmap;
					pBitmap->Attach(hBitmap);					
				}
			}
			delete pImage;
			GlobalFree(hGlobal);
		}
		return pBitmap;
	}

	CBitmap* LoadSkinFromZip(LPCTSTR szPicture)
	{
		if (szPicture == NULL || m_hZip == NULL) return NULL;

#ifdef _UNICODE
		ZIPENTRYW ze = {0};
#else
		ZIPENTRY ze = {0};
#endif
		CBitmap* pBitmap = NULL;
		int idx = 0;
		ZRESULT zr = FindZipItem(m_hZip, szPicture, FALSE, &idx, &ze);
		if (zr == ZR_OK)
		{
			HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
			if (hGlobal != NULL)
			{
				void* pVoid = GlobalLock(hGlobal);
				if (pVoid != NULL)
				{
					UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
					GlobalUnlock(hGlobal);
         
					IStream* pIStream = NULL;
					Gdiplus::Bitmap* pImage = NULL;
					DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
					if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
					{
						HBITMAP hBitmap = NULL;
						if (Gdiplus::Ok == pImage->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap) && hBitmap != NULL)
						{
							pBitmap = new CBitmap;
							pBitmap->Attach(hBitmap);					
						}
						delete pImage;
					}
					if (pIStream != NULL)
					{
						pIStream->Release();
					}
				}
				GlobalFree(hGlobal);
			}
		}

		return pBitmap;
	}
	HICON LoadIconFromZip(LPCTSTR szPicture)
	{
		if (szPicture == NULL || m_hZip == NULL) return NULL;

#ifdef _UNICODE
		ZIPENTRYW ze = {0};
#else
		ZIPENTRY ze = {0};
#endif
		HICON hIcon = NULL;
		int idx = 0;
		ZRESULT zr = FindZipItem(m_hZip, szPicture, FALSE, &idx, &ze);
		if (zr == ZR_OK)
		{
			HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
			if (hGlobal != NULL)
			{
				void* pVoid = GlobalLock(hGlobal);
				if (pVoid != NULL)
				{
					UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
					GlobalUnlock(hGlobal);

					CIcon ico;
					if(ico.CreateFromBuff((LPBYTE)pVoid, ze.unc_size))
					{ 
						hIcon = ico.GetHICON();
					}
					//IStream* pIStream = NULL;
					//Gdiplus::Bitmap* pImage = NULL;
					//DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
					//if (pIStream != NULL && (pImage = new Gdiplus::Bitmap(pIStream)) != NULL)
					//{
					//	if (Gdiplus::Ok == pImage->GetHICON(&hIcon) && hIcon != NULL)
					//	{
     //                       			
					//	}
					//}
					//if (pIStream != NULL)
					//{
					//	pIStream->Release();
					//}
				}
				GlobalFree(hGlobal);
			}
		}

		return hIcon;
	}

	BOOL ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath)
	{
		if (szItem == NULL || m_hZip == NULL) return FALSE;

#ifdef _UNICODE
		ZIPENTRYW ze = {0};
#else
		ZIPENTRY ze = {0};
#endif
		HICON hIcon = NULL;
		BOOL  bRet = FALSE;
		int idx = 0;
		ZRESULT zr = FindZipItem(m_hZip, szItem, FALSE, &idx, &ze);
		if (zr == ZR_OK)
		{
			HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
			if (hGlobal != NULL)
			{
				void* pVoid = GlobalLock(hGlobal);
				if (pVoid != NULL)
				{
					UnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
					GlobalUnlock(hGlobal);
					HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) 
					{
						DWORD dwByteWrite = 0;
						if (WriteFile(hFile, pVoid, ze.unc_size, &dwByteWrite, NULL) && dwByteWrite == ze.unc_size)
						{
							bRet = TRUE;
						}
						SetEndOfFile(hFile);
						CloseHandle(hFile);
					}
				}
				GlobalFree(hGlobal);
			}
		}

		return bRet;
	}
private:
	TCHAR szFile[MAX_PATH];
	HZIP  m_hZip;
	char* m_pZipFile;
};

inline CBitmap* AfxGetSkinImage(LPCTSTR szZipFileName)
{
	CBitmap* pBitmap = CZipSkin::getInstance()->LoadSkinFromZip(szZipFileName);
	if (pBitmap == NULL)
	{
		pBitmap = CZipSkin::getInstance()->LoadSkinFromFile(szZipFileName);
	}
	return pBitmap;
}

inline HICON AfxGetIconImage(LPCTSTR szZipFileName)
{
	HICON hIcon = CZipSkin::getInstance()->LoadIconFromZip(szZipFileName);
	return hIcon;
}

inline CBitmap* AfxGetSkinImage(HICON hIcon)
{
	return CZipSkin::getInstance()->LoadSkinFromHIcon(hIcon);
}

inline CBitmap* AfxGetSkinImage(DWORD dwResId)
{
	return CZipSkin::getInstance()->LoadSkinFromResId(dwResId);
}

inline void AfxDrawImage(CDC* pDC, CBitmap* pBitmap, CRect& rc)
{
	if (pBitmap == NULL)
		return ;

	BITMAP bmp = {0};
	pBitmap->GetBitmap(&bmp);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(pBitmap);
	if (bmp.bmWidth == rc.Width() && bmp.bmHeight == rc.Height())
	{
		pDC->BitBlt(rc.left, rc.top, bmp.bmWidth, bmp.bmHeight, &dcMem, 0, 0, SRCCOPY);
	}
	else
	{
		pDC->StretchBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	//BLENDFUNCTION blend;
	//memset( &blend, 0, sizeof( blend) );
	//blend.BlendOp= AC_SRC_OVER;
	//blend.SourceConstantAlpha= 0; // 透明度 最大255
	//pDC->AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, blend);
	dcMem.DeleteDC();
}

inline void AfxDrawImage(CDC* pDC, LPCTSTR szFileName, CRect& rc)
{
	CBitmap* pBitmap = AfxGetSkinImage(szFileName);
	if (pBitmap)
	{
		AfxDrawImage(pDC, pBitmap, rc);
		pBitmap->DeleteObject();
		delete pBitmap;
	}
}

inline void AfxDrawImage(CDC* pDC, DWORD dwResId, CRect& rc)
{
	CBitmap* pBitmap = AfxGetSkinImage(dwResId);
	if (pBitmap)
	{
		AfxDrawImage(pDC, pBitmap, rc);
		pBitmap->DeleteObject();
		delete pBitmap;
	}
}

inline void AfxDrawImage(CDC* pDC, HICON hIcon, CRect& rc)
{
	CBitmap* pBitmap = AfxGetSkinImage(hIcon);
	if (pBitmap)
	{
		AfxDrawImage(pDC, pBitmap, rc);
		pBitmap->DeleteObject();
		delete pBitmap;
	}
}

#ifndef __i8desk_pdisk__
void AfxMessageBoxEx(LPCTSTR lpszText);
#endif

class CButtonEx : public CButton
{
	friend class CDockStatusBar;
	DECLARE_DYNAMIC(CButtonEx)
	enum  BEX_Status
	{
		BEX_NORMAL,
		BEX_HOVER,
		BEX_PRESSED
	};
	enum  BDRAW_Status
	{
		BDRAW_NORMAL,
		BDRAW_ALPHA
	};
	enum BTYPE
	{
		BTYPE_NORMAL,
		BTYPE_NBTITLE,
		BTYPE_NBCONTENT,
		BTYPE_QUIKRUN
	};
public:
	enum BTEXT_STYLE
	{
		BTEXT_LEFT,
		BTEXT_CENTER,
		BTEXT_RIGHT
	};
public:
	CButtonEx();
	virtual ~CButtonEx();

	void  SetImage(DWORD backImage = 0, DWORD hoverImage = 0, DWORD selectImage = 0);
	void  SetImage(LPCTSTR backImage = NULL, LPCTSTR hoverImage = NULL, LPCTSTR selectImage = NULL, 
		LPCTSTR activeImage = NULL, LPCTSTR disableImage = NULL, BOOL bAlphaMode = FALSE);
	void  SetImage2(LPCTSTR backImage = NULL, LPCTSTR backImage2 = NULL, LPCTSTR backImage3 = NULL, 
		LPCTSTR selectImage = NULL, LPCTSTR selectImage2 = NULL, LPCTSTR selectImage3 = NULL);
	void  SetImage3(COLORREF backColor, COLORREF hoverColor, COLORREF selectColor, COLORREF activeColor);

	//设置图标（正常，按下）
	void SetIconImage(LPCTSTR iconImage, LPCTSTR iconSelImage = NULL);
	void SetIconImage(HICON hIcon, HICON hSelIcon = NULL);

	//设置按钮字体，大小，以及颜色等属性
	void  SetTextStyle(CString font = DEFAULT_FONT_NAME, int nSize = 12, 
		COLORREF clrText = RGB(0, 0, 0), COLORREF clrHover = RGB(0, 0, 0), COLORREF clrSel = RGB(0, 0, 0), COLORREF clrActive = RGB(0, 0, 0),
		BTEXT_STYLE nStyle = BTEXT_LEFT, BOOL bBold = FALSE, BOOL bUnderline = FALSE, BOOL bItalic = FALSE, BOOL bStrikeOut = FALSE,
		BTYPE bType = BTYPE_NORMAL, BOOL bSelBold = FALSE);
	void  SetActive(BOOL bActive);
	BOOL  GetActive()		{ return m_bActive; }
	void  SetCursor(HCURSOR hCursor)	{ m_hCursor = hCursor; }
protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

protected:
	void         DrawFadeEffect();                            //绘制渐隐效果

private:
	BEX_Status m_Status;
	COLORREF m_bgBack, m_bgHover, m_bgPressed, m_bgActive;
	CBitmap *m_pbkImage, *m_pbkImage2, *m_pbkImage3;				//默认背景左，默认背景中（拉伸），默认背景右
	CBitmap *m_phoverImage, *m_phoverImage2, *m_phoverImage3;	//鼠标经过左，鼠标经过中（拉伸），鼠标经过右
	CBitmap *m_pselectImage, *m_pselectImage2, *m_pselectImage3;	//鼠标按下左，鼠标按下中（拉伸），鼠标按下右
	CBitmap *m_pActiveImage, *m_piconImage, *m_piconSelImage;	//按钮左边的图标（默认，按下)
	CBitmap *m_pdisableImage;
	CString m_szFont;													//按钮的字体
	int m_nFontSize;
	COLORREF m_clrText, m_clrHover, m_clrSel, m_clrActive;
	BTEXT_STYLE m_nBTStyle;
	BOOL m_bBold, m_bUnderline, m_bItalic, m_bStrictOut;
	BOOL m_bActive;
	BOOL m_bSelBold;
	HCURSOR m_hCursor;
	BOOL m_bAlphaMode;
	BDRAW_Status m_DrawStatus;
	BTYPE m_bType;
	BLENDFUNCTION m_bf;
	CBitmap* m_pSourceBitmap;
	CToolTipCtrl m_toolTip;
	CString  m_strTooltip;                                    //网吧地址TOOLTIP，用于解决在单击后无法响应的变量
	HICON m_hIconNormal;
};

class CDockProgress : public CButtonEx
{
	DECLARE_DYNAMIC(CDockProgress)
public:
	CDockProgress();
	virtual ~CDockProgress();
public:
	void  SetHorzScroll(BOOL bHorz = TRUE);
	BOOL  IsHorzScroll()	{ return m_bIsHorz; }
	void  SetProgres(float fProgress /* 0.0 - 100.0 */);
	float GetProgress()	{ return m_fProgress; }

	void  SetImage(LPCTSTR lpszBkImage = NULL, LPCTSTR lpszFgImage = NULL);
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	DECLARE_MESSAGE_MAP();
private:
	BOOL  m_bIsHorz;
	float m_fProgress;
	CBitmap* m_pbkImage, *m_pfgImage;
};

class CDockHtmlCtrl : public CHtmlView
{
	DECLARE_DYNAMIC(CDockHtmlCtrl)
public:
	CDockHtmlCtrl();
	virtual ~CDockHtmlCtrl();
public:
	void SetNoScroll();
private:
	BOOL m_bScroll;
	// 	//实现IOleCommandTarget接口。
	// 	BEGIN_INTERFACE_PART(OleCommandTarget, IOleCommandTarget)
	// 		STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD *prgCmds, OLECMDTEXT *pCmdText);
	// 		STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);
	// 	END_INTERFACE_PART(OleCommandTarget)
	// 	DECLARE_INTERFACE_MAP()

	//处理IE事件
	virtual void	OnTitleChange(LPCTSTR lpszText);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *info);
	virtual void	OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
		LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);	

	//过滤掉Doc/View的一些处理。
	virtual void PostNcDestroy() {  }
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP();
};

class CDockHtmlCtrlEx : public CStatic
{
	DECLARE_DYNAMIC(CDockHtmlCtrlEx)
public:
	CDockHtmlCtrlEx();
	virtual ~CDockHtmlCtrlEx();

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
private:
	CDockHtmlCtrl m_WebPage;
};

class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)
public:
	CEditEx();
	virtual ~CEditEx();
public:
	void SetDefaultText(LPCTSTR lpszDef) { m_strDef = lpszDef; }
	CString GetDefaultText() { return m_strDef; }
protected:
	void xProcess();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()
private:
	CString m_strDef;
};

class CDockComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CDockComboBox)
public:
	CDockComboBox();
	virtual ~CDockComboBox();
public:
	CString GetText()
	{
		CString str;
		if (m_Edit.GetSafeHwnd() != NULL)
			m_Edit.GetWindowText(str);

		return str;
	}
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
private:
	CBitmap* m_pbkImage;
	CBitmap* m_pRight;
	CEditEx  m_Edit;
};

typedef struct tagNavClick
{
	DWORD dwIdx;
	DWORD dwClick;
	DWORD dwRunTime;
}tagNavClick;

class CDockNavBar : public CStatic
{
	DECLARE_DYNAMIC(CDockNavBar)
public:
	CDockNavBar();
	~CDockNavBar();
public:
	void SetActive(int idx);
	int  GetActive() { return m_nActive; }
	void SetAllUnActive();
	void SetDockMini(BOOL bIsMini);
	BOOL GetIsDockMini()	{ return m_bIsDockMini; }
	void SetNetBarText(CString& str);
	void GetAllNavClick(tagNavClick* pNavClick);
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bIsDockMini;
	int m_nActive;
	CBitmap *m_pbkImage, *m_pbkLogoImage, *m_pbkTipImage, *m_pHeadRightImage;
	CButtonEx m_btnNetBar;
	CButtonEx m_btnNav[12];
	CButtonEx m_btnTool[6];
	//统计6个导航栏的点击数，以及时长。（ID是1-6）
	tagNavClick  m_NavClicks[6];
};

class CDockBrowseBar : public CStatic
{
	DECLARE_DYNAMIC(CDockBrowseBar)
public:
	CDockBrowseBar();
	~CDockBrowseBar();
public:
	CString GetText();
	void SetText(CString &str);
	void SetListText(std::vector<CString>& lst);
	void SetNoScroll();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
	CDockComboBox m_CtrlUrl;
	CBitmap* m_pbkImage;
	CButtonEx m_BtnGroup[13];
};

class CDockFinderBar : public CStatic
{
	DECLARE_DYNAMIC(CDockFinderBar)
public:
	CDockFinderBar();
	~CDockFinderBar();
public:
	CString GetText();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
	CEditEx m_edit;
	CBitmap* m_pbkImage;
	CBitmap* m_pbkLeft;
	CBitmap* m_pbkRight;
	CBitmap* m_pbAlpha;
	CBitmap* m_pbAlphaLeft;
	CBitmap* m_pbAlphaRight;

	CButtonEx m_BtnGroup[29];
};

class CDockTabBar : public CStatic
{
	DECLARE_DYNAMIC(CDockTabBar)
public:
	CDockTabBar();
	~CDockTabBar();
public:
	CButtonEx* AddTab(LPCTSTR lpszCap, LPCTSTR iconImage, LPCTSTR iconSelImage, BOOL bRefresh = FALSE);
	CString  GetTabText(int index);
	DWORD    GetTabCount()	 { return m_BtnTabGroup.size(); }
	void RemoveAllTab();
	void ShowUrlNav(BOOL bShow);
	void SetActive(int idx);
	int  GetActive() { return m_nActive; }
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()

private:
	void      GetTabSkinColor();
private:
	CBitmap *m_pbkImage;
	int m_nActive;
	CButtonEx m_btnFind, m_btnBoderLeft, m_btnBorderRight;
	std::vector<CButtonEx*> m_BtnTabGroup;
	COLORREF  m_clrNormal, m_clrHover, m_clrSel;
};

class CDockStatusBar : public CStatic
{
	DECLARE_DYNAMIC(CDockStatusBar)
public:
	CDockStatusBar();
	~CDockStatusBar();
public:
	void SetQuickRunGroup(int nIndex/*0--4*/, HICON hIcon, CString strCaption);
	void AddScroolText(LPCTSTR szScrollText, LPCTSTR url);
	void RemoveScroolText(LPCTSTR szScroolText);
	void RemoveAllScroolText();
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()
private:
	CBitmap* m_pbkImage;
	CBitmap* m_pQuickImage;
	CButtonEx m_BtnGroup[15];
	CDockHtmlCtrl   m_WebPage;
};

class CDockHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CDockHeaderCtrl)
public:
	CDockHeaderCtrl();
	virtual ~CDockHeaderCtrl();
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

class CDockListCtrl : public CListCtrl
{
	typedef struct tagCellColor
	{
		COLORREF clrText;
		COLORREF clrBkText;
		COLORREF clrSelText;
		COLORREF clrSelBkText;
	}tagCellColor;
	DECLARE_DYNAMIC(CDockListCtrl)
public:
	CDockListCtrl();
	virtual ~CDockListCtrl();
public:
	void SetCellColor(int nRow, int nCol, COLORREF clrText, COLORREF clrBkCol, 
		COLORREF clrSelText, COLORREF clrSelBkCol);
	void GetCellColr(int nRow, int nCol, COLORREF& clrText, COLORREF& clrBkCol, 
		COLORREF& clrSelText, COLORREF& clrSelBkCol);
protected:
	afx_msg void OnCustomDrawList (NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	std::map<DWORD, tagCellColor*> m_CellColor;
	CDockHeaderCtrl m_headCtrl;
	CFont m_headFont;
};


//标准对话框基类（用于带透明效果的对话框）
class CStandardDialogBase : public CDialog
{
	DECLARE_DYNAMIC(CStandardDialogBase)
public:
	CStandardDialogBase(UINT nIDTemplate, COLORREF clrTransparent = RGB(0, 255, 0), CWnd* pParentWnd = NULL);
	virtual ~CStandardDialogBase();

public:
	void			ShowDialog();

protected:
	void			EndDialog(int nResult);
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL	OnInitDialog();
	virtual void    OnOK();
	virtual void    OnCancel();

	DECLARE_MESSAGE_MAP()
private:
	COLORREF      m_clrTransparent;
	CBrush        m_brush;
};

//透明对话框基类
class CTransDialogBase : public CDialog
{
	DECLARE_DYNAMIC(CTransDialogBase)
public:
	CTransDialogBase(UINT nIDTemplate, LPCTSTR lpszImageFile, CWnd* pParent = NULL, void* pParam = NULL);
	virtual ~CTransDialogBase();

public:
	virtual CStandardDialogBase* CreateStandardDialog(void* pParam = NULL) = 0;	
	CStandardDialogBase*         GetStandardDialog();
	void                         UpdateView();

protected:
	afx_msg BOOL                OnEraseBkgnd(CDC* pDC);
	afx_msg void                OnMove(int x, int y);
	afx_msg void                OnSize(UINT nType, int cx, int cy);
	afx_msg int                 OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void                OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL                OnInitDialog();
	virtual void                OnOK();
	virtual void                OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	BLENDFUNCTION               m_bf;
	CBitmap                     *m_pImage;
	CStandardDialogBase         *m_pStandardDialog;
	void                        *m_pStandardDialogParam;
};