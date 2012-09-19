#pragma once

namespace I8SkinCtrl_ns
{
    class CI8CtrlBackground
    {
    public:
        inline virtual ~CI8CtrlBackground(){}
        virtual void OnDraw(CWnd* pWnd, CDC* pDC, const CRect& rtRect) = 0;
        inline virtual CRect GetValidRect(const CRect& rtRect) const
        {
            return rtRect;
        }
    };
}
