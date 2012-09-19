#pragma once

#include "I8Bitmap.h"

namespace I8SkinCtrl_ns
{
    class CI8BGCache
    {
        CI8Bitmap m_clImage;
        CSize m_clSize;
        BOOL m_bNeedRedraw;
        CRgn m_clRgn;
    public:
        CI8BGCache();
        void SetSize(const CSize& clSize);
        void NeedRedraw(void);
        BOOL IsNeedRedraw(void) const;
        CI8Bitmap& GetImage(BOOL bForRedraw = TRUE);
        void Draw(HDC hDC) const;
        void UpdateRgn(void);
        const CRgn& GetRgn(void) const;
        BOOL IsNull(void) const;
    };
}
