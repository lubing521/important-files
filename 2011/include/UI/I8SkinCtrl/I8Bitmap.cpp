#include "StdAfx.h"
#include "I8Bitmap.h"
#include "I8Trusteeship.h"

CI8Bitmap::CI8Draw::CI8Draw()
    : m_hDC(NULL)
{
}

CI8Bitmap::CI8Draw::~CI8Draw()
{
    if (m_hDC)
    {
        DeleteDC(m_hDC);
    }
}

void CI8Bitmap::CI8Draw::Set(HDC hDC)
{
    if (m_hDC)
    {
        DeleteDC(m_hDC);
    }
    m_hDC = hDC;
}

CI8Bitmap::CI8Bitmap(void)
    : m_hBitmap(NULL)
    , m_bAutoFree(FALSE)
{
}

CI8Bitmap::CI8Bitmap(HBITMAP hBitmap, BOOL bAutoFree)
    : m_hBitmap(hBitmap)
    , m_bAutoFree(bAutoFree)
{
}

CI8Bitmap::CI8Bitmap(const CI8Bitmap& clBitmap)
    : m_hBitmap(NULL)
    , m_bAutoFree(FALSE)
{
    assert(!clBitmap.m_bAutoFree);
    m_hBitmap = clBitmap.m_hBitmap;
    m_bAutoFree = clBitmap.m_bAutoFree;
}

CI8Bitmap::~CI8Bitmap(void)
{
    Clear();
}


CI8Bitmap& CI8Bitmap::operator=(const CI8Bitmap& clBitmap)
{
    if (&clBitmap != this)
    {
        assert(!clBitmap.m_bAutoFree);
        Clear();
        m_hBitmap = clBitmap.m_hBitmap;
        m_bAutoFree = clBitmap.m_bAutoFree;
    }
    return *this;
}

void CI8Bitmap::Set(HBITMAP hBitmap, BOOL bAutoFree)
{
    if (hBitmap != m_hBitmap)
    {
        Clear();
        if (hBitmap != NULL)
        {
            m_bAutoFree = bAutoFree;
            m_hBitmap = hBitmap;
        }
    }
}

void CI8Bitmap::Clear()
{

    if (m_hBitmap != NULL)
    {
        if (m_bAutoFree)
        {
            DeleteObject(m_hBitmap);
        }
        m_hBitmap = NULL;
        m_bAutoFree = FALSE;
    }
}

void CI8Bitmap::Draw(HDC hDC, const CPoint& clPos) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    CSize clSize = GetSize();
    BitBlt(hDC, clPos.x, clPos.y, clSize.cx, clSize.cy, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
}

void CI8Bitmap::Draw(HDC hDC, const CRect& clRect) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    CSize clSize = GetSize();
    StretchBlt(hDC, clRect.left, clRect.top, clRect.Width(), clRect.Height(), hMemDC, 0, 0, clSize.cx, clSize.cy, SRCCOPY);
    DeleteDC(hMemDC);
}

void CI8Bitmap::Draw(HDC hDC, const CRect& clDesRect, const CRect& clSrcRect) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    StretchBlt(hDC, clDesRect.left, clDesRect.top, clDesRect.Width(), clDesRect.Height(), hMemDC, clSrcRect.left, clSrcRect.top, clSrcRect.Width(), clSrcRect.Height(), SRCCOPY);
    DeleteDC(hMemDC);
}

void CI8Bitmap::AlphaBlend(HDC hDC, const CPoint& clPos, int iSrcAlpha) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    BLENDFUNCTION stInfo = {AC_SRC_OVER, 0, iSrcAlpha, AC_SRC_ALPHA};
    CSize clSize = GetSize();
    ::AlphaBlend(hDC, clPos.x, clPos.y, clSize.cx, clSize.cy, hMemDC, 0, 0, clSize.cx, clSize.cy, stInfo);
    DeleteDC(hMemDC);
}

void CI8Bitmap::AlphaBlend(HDC hDC, const CRect& clRect, int iSrcAlpha) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    BLENDFUNCTION stInfo = {AC_SRC_OVER, 0, iSrcAlpha, AC_SRC_ALPHA};
    CSize clSize = GetSize();
    ::AlphaBlend(hDC, clRect.left, clRect.top, clRect.Width(), clRect.Height(), hMemDC, 0, 0, clSize.cx, clSize.cy, stInfo);
    DeleteDC(hMemDC);
}

void CI8Bitmap::AlphaBlend(HDC hDC, const CPoint& clPos, const CRect& clSrcRect, int iSrcAlpha) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    BLENDFUNCTION stInfo = {AC_SRC_OVER, 0, iSrcAlpha, AC_SRC_ALPHA};
    CSize clSize = GetSize();
    ::AlphaBlend(hDC, clPos.x, clPos.y, clSrcRect.Width(), clSrcRect.Height(), hMemDC, clSrcRect.left, clSrcRect.top, clSrcRect.Width(), clSrcRect.Height(), stInfo);
    DeleteDC(hMemDC);
}

void CI8Bitmap::AlphaBlend(HDC hDC, const CRect& clDesRect, const CRect& clSrcRect, int iSrcAlpha) const
{
    assert(hDC != NULL);
    assert(m_hBitmap != NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    BLENDFUNCTION stInfo = {AC_SRC_OVER, 0, iSrcAlpha, AC_SRC_ALPHA};
    CSize clSize = GetSize();
    ::AlphaBlend(hDC, clDesRect.left, clDesRect.top, clDesRect.Width(), clDesRect.Height(), hMemDC, clSrcRect.left, clSrcRect.top, clSrcRect.Width(), clSrcRect.Height(), stInfo);
    DeleteDC(hMemDC);
}

CSize CI8Bitmap::GetSize() const
{
    assert(m_hBitmap != NULL);
    BITMAP stBitmap;
    GetObject(m_hBitmap, sizeof(stBitmap), &stBitmap);
    return CSize(stBitmap.bmWidth, stBitmap.bmHeight);
}

void CI8Bitmap::Create(const CSize& clSize)
{
    Clear();
    CI8Trusteeship<UINT> clPixels(clSize.cx * clSize.cy);
    clPixels.SetAll(0);
    m_hBitmap = CreateBitmap(clSize.cx, clSize.cy, 1, 32, clPixels.GetPointer());
    m_bAutoFree = TRUE;
    assert(m_hBitmap != NULL);
}

BOOL CI8Bitmap::IsNull() const
{
    return m_hBitmap == NULL;
}

CI8Trusteeship<UINT> CI8Bitmap::GetPixels() const
{
    assert(m_hBitmap != NULL);
    CSize clSize = GetSize();
    CI8Trusteeship<UINT> clPixels;
    clPixels.ApplyArray(clSize.cx * clSize.cy);
    GetBitmapBits(m_hBitmap, clPixels.GetCount() * sizeof(UINT), clPixels.Pointer());
    return clPixels;
}

HDC CI8Bitmap::GetDC(CI8Draw& clDraw, HDC hDC)
{
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, m_hBitmap);
    clDraw.Set(hMemDC);
    return hMemDC;
}

