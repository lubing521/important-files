#pragma once

#include "I8CtrlBackground.h"
#include "I8BGImage.h"

namespace I8SkinCtrl_ns
{
    class CI8SkinCtrlBG : public CI8CtrlBackground
    {
        CI8BGImage m_clBGImage;
    public:
        inline virtual ~CI8SkinCtrlBG()
        {
        }
        virtual void OnDraw(CWnd* pWnd, CDC* pDC, const CRect& rtRect)
        {
            if (!m_clBGImage.IsNull())
            {
                m_clBGImage.Draw(*pDC, rtRect);
            }
            else
            {
                pDC->FillSolidRect(&rtRect, GetSysColor(COLOR_MENU));
            }
        }
        inline virtual CRect GetValidRect(const CRect& rtRect) const
        {
            return m_clBGImage.GetSpaceArea(rtRect);
        }
        inline void SetBackground(const stdex::tString& sImage, const stdex::tString& sOption, const stdex::tString& sOptionName)
        {
            m_clBGImage.Load(sImage, sOption, sOptionName);
        }
        inline void SetBackground(const stdex::tString& sSkinName)
        {
            m_clBGImage.Load(sSkinName);
        }
    };
}
