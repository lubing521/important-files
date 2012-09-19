#pragma once

#include "I8SkinPack.h"

namespace I8SkinCtrl_ns
{
    BOOL I8_ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath);

    CBitmap* I8_GetSkinImage(LPCTSTR szZipFileName);

    HICON I8_GetIconImage(LPCTSTR szZipFileName);

    CBitmap* I8_GetSkinImage(HICON hIcon);

    CBitmap* I8_GetSkinImage(DWORD dwResId);

    void I8_DrawImage(CDC* pDC, CBitmap* pBitmap, CRect& rc);

    void I8_DrawImage(CDC* pDC, LPCTSTR szFileName, CRect& rc);

    void I8_DrawImage(CDC* pDC, DWORD dwResId, CRect& rc);

    void I8_DrawImage(CDC* pDC, HICON hIcon, CRect& rc);

    void I8_FillParentWndBG(HWND hWnd, HDC hDC, CBrush& clBrush);

    CSize I8_GetTextSize(HFONT hFont, const stdex::tString& sText);
}