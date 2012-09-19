#pragma once

#include "I8ItemListB.h"

namespace I8SkinCtrl_ns
{
    class CI8TabCtrl : public CI8ItemListB
    {
    private:
        enum TAB_ITEM_STATE_EN
        {
            E_TAB_ITEM_SELECTED = 0,
            E_TAB_ITEM_UNSELECTED,
            E_TAB_ITEM_SELECTED_HOVER,
            E_TAB_ITEM_UNSELECTED_HOVER,
            E_TAB_ITEM_STATE_MAX
        };
        struct ResData_st
        {
            CI8BGImage aclTabItemImage[E_TAB_ITEM_STATE_MAX];
            HFONT hFont;
            UINT anFontColor[E_TAB_ITEM_STATE_MAX];
            int iFontTop;
        };
        class CI8TabItem : public CI8ItemList::CI8Item
        {
            ResData_st& m_stResData;
            stdex::tString m_sText;
            CWnd* m_pWnd;
            mutable CSize m_clItemSize;
        public:
            CI8TabItem(ResData_st& stResData, const stdex::tString& sText, CWnd* pWnd);
            virtual ~CI8TabItem();
        protected:;
            virtual void OnDraw(CDC* pDC, const ItemInfo_st& stItemInfo);
            virtual void OnSelect(const ItemInfo_st& stItemInfo);
            virtual void OnUnselect(const ItemInfo_st& stItemInfo);
            virtual void OnAdd(const ItemInfo_st& stItemInfo);
            virtual void OnDeleted(const ItemInfo_st& stItemInfo);
            TAB_ITEM_STATE_EN GetState(const ItemInfo_st& stItemInfo) const;
            virtual CSize GetItemSize(int iIndex) const;
        };
        class CI8CalculateTabItemRect : public CI8ItemList::CI8CalculateItemRect
        {
            CI8TabCtrl* m_pclTabCtrl;
            CRect m_rtValidRect;
        public:
            CI8CalculateTabItemRect(CI8TabCtrl* m_pclTabCtrl);
            virtual ~CI8CalculateTabItemRect();
            virtual void Begin(const CRect& rtClient);
            virtual void CalculateItem(int iIndex, CI8ItemList::CI8Item* pclItem, CRect& rtItem, BOOL& bVisible);
        };
        friend class CI8CalculateIconItemRect;
    private:
        DECLARE_DYNAMIC(CI8TabCtrl)
    private:
        CI8CalculateTabItemRect m_clCalculateTabItemRect;
        ResData_st m_stResData;
        int m_iItemHoriSpace;
        int m_iLeftSpace;
        int m_iRightSpace;
    public:
        CI8TabCtrl();
        virtual ~CI8TabCtrl();
        void Load(const stdex::tString& sSkinName);
        void AddItem(const stdex::tString& sText, CWnd* pWnd = NULL, BOOL bUpdate = TRUE);
    };
}
