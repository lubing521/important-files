#pragma once

#include "I8ItemListB.h"

namespace I8SkinCtrl_ns
{
    class CI8SimpleIconView : public CI8ItemList
    {
    private:
        class CI8CalculateIconItemRect : public CI8ItemList::CI8CalculateItemRect
        {
            CI8SimpleIconView* m_pclIconView;
            int m_iColumnCount;
            int m_iRowCount;
            int m_iItemCountEveryPage;
            int m_iLeft;
            int m_iRight;
            int m_iValidItemBeginIndex;
            int m_iValidItemEndIndex;
        public:
            CI8CalculateIconItemRect(CI8SimpleIconView* pclIconView);
            virtual ~CI8CalculateIconItemRect();
            virtual void Begin(const CRect& rtClient);
            virtual void CalculateItem(int iIndex, CI8ItemList::CI8Item* pclItem, CRect& rtItem, BOOL& bVisible);
        };
        friend class CI8CalculateIconItemRect;
    private:
        DECLARE_DYNAMIC(CI8SimpleIconView)
        DECLARE_MESSAGE_MAP()
    private:
        CI8CalculateIconItemRect m_clCalculateIconItemRect;
        CSize m_clItemSize;
        int m_iItemHoriSpace;
        int m_iItemVertSpace;
        int m_iVertTotalLength;
        int m_iVertPageLength;
        int m_iCurVertPos;
        CScrollBar* m_pVertScroll;
    public:
        CI8SimpleIconView(BOOL bAutoFreeItem, CI8CtrlBackground* pclBackground = NULL);
        virtual ~CI8SimpleIconView();
        void SetItemSize(const CSize& clItemSize, const CSize& clItemSpace);
        void SetVertPos(int iPos);
        int GetCurVertPos() const;
    protected:
        virtual void Init();
        afx_msg void OnDestroy();
        afx_msg void OnSize(IN UINT nType, IN int cx, IN int cy);
        afx_msg void OnVScroll(UINT nSBCode,UINT nPos,CScrollBar *pVScroll);
        virtual CScrollBar* ApplyVertScroll();
        virtual void OnUpdateItemRect();
        virtual CRect GetItemArea(const CRect& rtRect) const;
    };
}
