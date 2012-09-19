#include "StdAfx.h"
#include "I8BGCache.h"
#include "I8Trusteeship.h"

using namespace I8SkinCtrl_ns;

CI8BGCache::CI8BGCache(void)
    : m_clSize(0, 0)
    , m_bNeedRedraw(FALSE)
{
}

void CI8BGCache::SetSize(const CSize& clSize)
{
    m_clSize = clSize;
    m_bNeedRedraw = TRUE;
    m_clImage.Create(clSize);
}

void CI8BGCache::NeedRedraw(void)
{
    m_bNeedRedraw = TRUE;
}

BOOL CI8BGCache::IsNeedRedraw(void) const
{
    return m_bNeedRedraw;
}

CI8Bitmap& CI8BGCache::GetImage(BOOL bForRedraw)
{
    assert(!m_clImage.IsNull());
    if (bForRedraw)
    {
        m_bNeedRedraw = FALSE;
    }
    return m_clImage;
}

void CI8BGCache::Draw(HDC hDC) const
{
    assert(!m_clImage.IsNull());
    m_clImage.Draw(hDC);
}

void CI8BGCache::UpdateRgn(void)
{
    const int C_NULL_RECT_NUM = 200;
    CSize clSize = m_clImage.GetSize();
    int iRectMaxNum = C_NULL_RECT_NUM;
    CI8Trusteeship<CHAR> clRgnData;
    clRgnData.ApplyArray(sizeof(RGNDATAHEADER) + sizeof(RECT) * iRectMaxNum);
    RGNDATA* pstRgnData = reinterpret_cast<RGNDATA*>(clRgnData.Pointer());
    pstRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pstRgnData->rdh.iType = RDH_RECTANGLES;
    pstRgnData->rdh.nRgnSize = 0;
    pstRgnData->rdh.nCount = 0;
    CRect clBoundRect;
    CI8Trusteeship<UINT> clPixels = m_clImage.GetPixels();
    for (int j = 0; j < clSize.cy; ++j)
    {
        for (int i = 0; i < clSize.cx; ++i)
        {
            if ((clPixels[j * clSize.cx + i] & 0xFF000000) == 0)
            {
                if (int(pstRgnData->rdh.nCount) >= iRectMaxNum)
                {
                    iRectMaxNum += C_NULL_RECT_NUM;
                    clRgnData.Extend(sizeof(RGNDATAHEADER) + sizeof(RECT) * iRectMaxNum);
                    pstRgnData = reinterpret_cast<RGNDATA*>(clRgnData.Pointer());
                }
                RECT* pstRect = reinterpret_cast<RECT*>(pstRgnData->Buffer);
                CRect clNullRect(i, j, i + 1, j + 1);
                if (pstRgnData->rdh.nCount == 0)
                {
                    clBoundRect = clNullRect;
                }
                else
                {
                    if (clNullRect.left < clBoundRect.left)
                    {
                        clBoundRect.left = clNullRect.left;
                    }
                    if (clNullRect.right > clBoundRect.right)
                    {
                        clBoundRect.right = clNullRect.right;
                    }
                    if (clNullRect.top < clBoundRect.top)
                    {
                        clBoundRect.top = clNullRect.top;
                    }
                    if (clNullRect.bottom > clBoundRect.bottom)
                    {
                        clBoundRect.bottom = clNullRect.bottom;
                    }
                }
                pstRect[pstRgnData->rdh.nCount] = clNullRect;
                ++pstRgnData->rdh.nCount;
            }
        }
    }
    pstRgnData->rdh.rcBound = clBoundRect;
    if (m_clRgn.GetSafeHandle() != NULL)
    {
        m_clRgn.DeleteObject();
    }
    m_clRgn.CreateRectRgn(0, 0, clSize.cx, clSize.cy);
    if (pstRgnData->rdh.nCount > 0)
    {
        CRgn clDiffRgn;
        clDiffRgn.CreateFromData(NULL, sizeof(RGNDATAHEADER) + sizeof(RECT) * pstRgnData->rdh.nCount, pstRgnData);
        m_clRgn.CombineRgn(&m_clRgn, &clDiffRgn, RGN_DIFF);
    }
}

const CRgn& CI8BGCache::GetRgn(void) const
{
    return m_clRgn;
}

BOOL CI8BGCache::IsNull(void) const
{
    return m_clImage.IsNull();
}