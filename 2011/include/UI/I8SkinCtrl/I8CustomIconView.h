#pragma once

#include <list>
#include "ui/I8SkinCtrl/I8SimpleIconView.h"

namespace I8SkinCtrl_ns
{
    class CI8CustomIconView : public CI8SimpleIconView
    {
    private:
        struct ResData_st
        {
            HICON* phIcon;
            std::tr1::function<void(int iIconIndex)> m_pfnRequestIcon;
        };
        class CI8IconItem :
            public CI8SimpleIconView::CI8Item
        {
            ResData_st& m_stResData;
        public:
            CI8IconItem(ResData_st& stResData);
            virtual ~CI8IconItem(void);
            virtual void OnDraw(CDC* pDC, const CI8ItemList::ItemInfo_st& stItemInfo);
        };
    private:
        ResData_st m_stResData;
        CI8IconItem m_clSeeIcon;
    public:
        CI8CustomIconView(CI8CtrlBackground* pclBackground = NULL);
        void SetIcon(HICON* phIcon, int iCount);
        void Update();
        void SetDrawCallBack(std::tr1::function<void(int iIconIndex)> pfnRequestIcon);
    };
}