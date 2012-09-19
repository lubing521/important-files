#pragma once

#include "I8Trusteeship.h"
#include <memory>

class CI8Bitmap
{
public:
    class CI8Draw
    {
        HDC m_hDC;
    public:
        CI8Draw();
        ~CI8Draw();
        void Set(HDC hDC);
    private:
        CI8Draw(const CI8Draw&);
        CI8Draw& operator=(const CI8Draw&);
    };
private:
    HBITMAP m_hBitmap;
    BOOL m_bAutoFree;
public:
    CI8Bitmap(void);
    CI8Bitmap(HBITMAP hBitmap, BOOL bAutoFree);
    ~CI8Bitmap(void);
    void Set(HBITMAP hBitmap, BOOL bAutoFree);
    void Clear();
    void Create(const CSize& clSize);
    void Draw(HDC hDC, const CPoint& clPos = CPoint(0, 0)) const;
    void Draw(HDC hDC, const CRect& clRect) const;
    void Draw(HDC hDC, const CRect& clDesRect, const CRect& clSrcRect) const;
    void AlphaBlend(HDC hDC, const CPoint& clPos = CPoint(0, 0), int iSrcAlpha = 255) const;
    void AlphaBlend(HDC hDC, const CRect& clRect, int iSrcAlpha = 255) const;
    void AlphaBlend(HDC hDC, const CPoint& clPos, const CRect& clSrcRect, int iSrcAlpha = 255) const;
    void AlphaBlend(HDC hDC, const CRect& clDesRect, const CRect& clSrcRect, int iSrcAlpha = 255) const;
    CSize GetSize() const;
    BOOL IsNull() const;
    HDC GetDC(CI8Draw& clDraw, HDC hDC);
    CI8Trusteeship<UINT> GetPixels() const;
    operator HBITMAP() const {return m_hBitmap;}
protected:
    CI8Bitmap(const CI8Bitmap& clBitmap);
    CI8Bitmap& operator=(const CI8Bitmap& clBitmap);
};
