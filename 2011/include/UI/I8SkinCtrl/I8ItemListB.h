#pragma once

#include "I8ItemList.h"
#include "I8SkinCtrlBG.h"

namespace I8SkinCtrl_ns
{
    class CI8ItemListB : public CI8ItemList
    {
    private:
        CI8SkinCtrlBG m_clBackground;
    public:
        inline CI8ItemListB(CI8CalculateItemRect& clCalculateItemRect, BOOL bAutoFreeItem)
            : CI8ItemList(clCalculateItemRect, bAutoFreeItem, &m_clBackground)
        {
        }
        inline virtual ~CI8ItemListB()
        {
        }
        inline void SetBackground(const stdex::tString& sImage, const stdex::tString& sOption, const stdex::tString& sOptionName)
        {
            m_clBackground.SetBackground(sImage, sOption, sOptionName);
            if (GetSafeHwnd() != NULL)
            {
                NeedRedraw();
                Invalidate();
            }
        }
        inline void SetBackground(const stdex::tString& sSkinName)
        {
            m_clBackground.SetBackground(sSkinName);
            if (GetSafeHwnd() != NULL)
            {
                NeedRedraw();
                Invalidate();
            }
        }
    };
}
