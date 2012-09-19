#include "stdafx.h"
#include "I8SkinCommonFunction.h "

#include "../ImageHelpers.h"

using namespace I8SkinCtrl_ns;

CBitmap* I8SkinCtrl_ns::I8_GetSkinImage(LPCTSTR szZipFileName)
{
    CBitmap* pBitmap = CI8SkinPack::Instance().LoadSkinFromZip(szZipFileName);
    if (pBitmap == NULL)
    {
        pBitmap = CI8SkinPack::Instance().LoadSkinFromFile(szZipFileName);
    }
    return pBitmap;
}

HICON I8SkinCtrl_ns::I8_GetIconImage(LPCTSTR szZipFileName)
{
    HICON hIcon = CI8SkinPack::Instance().LoadIconFromZip(szZipFileName);
    return hIcon;
}

CBitmap* I8SkinCtrl_ns::I8_GetSkinImage(HICON hIcon)
{
    return CI8SkinPack::Instance().LoadSkinFromHIcon(hIcon);
}

CBitmap* I8SkinCtrl_ns::I8_GetSkinImage(DWORD dwResId)
{
    return CI8SkinPack::Instance().LoadSkinFromResId(dwResId);
}

void I8SkinCtrl_ns::I8_DrawImage(CDC* pDC, CBitmap* pBitmap, CRect& rc)
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
    dcMem.DeleteDC();
}

void I8SkinCtrl_ns::I8_DrawImage(CDC* pDC, LPCTSTR szFileName, CRect& rc)
{
    CBitmap* pBitmap = I8_GetSkinImage(szFileName);
    if (pBitmap)
    {
        I8_DrawImage(pDC, pBitmap, rc);
        pBitmap->DeleteObject();
        delete pBitmap;
    }
}

void I8SkinCtrl_ns::I8_DrawImage(CDC* pDC, DWORD dwResId, CRect& rc)
{
    CBitmap* pBitmap = I8_GetSkinImage(dwResId);
    if (pBitmap)
    {
        I8_DrawImage(pDC, pBitmap, rc);
        pBitmap->DeleteObject();
        delete pBitmap;
    }
}

void I8SkinCtrl_ns::I8_DrawImage(CDC* pDC, HICON hIcon, CRect& rc)
{
    CBitmap* pBitmap = I8_GetSkinImage(hIcon);
    if (pBitmap)
    {
        I8_DrawImage(pDC, pBitmap, rc);
        pBitmap->DeleteObject();
        delete pBitmap;
    }
}

void I8SkinCtrl_ns::I8_FillParentWndBG(HWND hWnd, HDC hDC, CBrush& clBrush)
{
    if (clBrush.GetSafeHandle() == NULL)
    {
        assert(GetParent(hWnd) != NULL && "按钮必须有父窗口。");
        HBRUSH hBrush = AtlGetBackgroundBrush(hWnd, GetParent(hWnd));
        clBrush.Attach(hBrush);
    }
    HBRUSH hOldBrush = static_cast<HBRUSH>(::SelectObject(hDC, clBrush));
    CRect rtClient;
    GetClientRect(hWnd, &rtClient);
    ::PatBlt(hDC, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), PATCOPY);
    SelectObject(hDC, hOldBrush);
}

CSize I8SkinCtrl_ns::I8_GetTextSize(HFONT hFont, const stdex::tString& sText)
{
    assert(hFont != NULL);
    HWND hWnd = GetDesktopWindow();
    HDC hDC = GetWindowDC(hWnd);
    CRect rtText;
    DrawText(hDC, sText.c_str(), sText.size(), &rtText, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);
    ReleaseDC(hWnd, hDC);
    return rtText.Size();
}

BOOL I8SkinCtrl_ns::I8_ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath)
{
    return CI8SkinPack::Instance().ExtractItem(szItem, szFilePath);
}