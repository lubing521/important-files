#pragma once

#include "I8SkinBitmap.h"
#include "../../Extend STL/UnicodeStl.h"

namespace I8SkinCtrl_ns
{
    class CI8BGImage
    {
    private:
        CI8SkinBitmap m_clImage;
        int m_iTopLeft;
        int m_iTopRight;
        int m_iTop;
        int m_iLeft;
        int m_iRight;
        int m_iBottomLeft;
        int m_iBottomRight;
        int m_iBottom;
    public:
        CI8BGImage(void);
        void Load(const stdex::tString& sName, BOOL bCheckExist = TRUE);
        void Load(const stdex::tString& sImage, const stdex::tString& sOption, const stdex::tString& sOptionName, BOOL bCheckExist = TRUE);
        void Draw(HDC hDC, const CRect& rtRect) const;
        CRect GetSpaceArea(const CRect& rtRect) const;
        int GetLeftSpace() const;
        int GetTopSpace() const;
        int GetRightSpace() const;
        int GetBottomSpace() const;
        CSize GetMidSize() const;
        BOOL IsNull() const;
        CSize GetSize() const;
    };
}
