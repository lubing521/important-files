#pragma once

#include "I8SkinPack.h"

namespace I8SkinCtrl_ns
{
    class CI8IconView : public CListCtrl
    {
    public:
        class CI8Item
        {
            friend class CI8IconView;
            const CI8IconView* m_pclListCtrl;
            BOOL m_bSelected;
        public:
            inline CI8Item() : m_bSelected(FALSE){}
            inline virtual ~CI8Item(){}
        protected:
            inline BOOL IsSelected() const{return m_bSelected;}
            inline const CI8IconView& GetListCtrl() const{return *m_pclListCtrl;}
        private:
            inline void Set(const CI8IconView* pclListCtrl, BOOL bSelected){m_pclListCtrl = pclListCtrl; m_bSelected = bSelected;}
        };
        class CI8IconItem : public CI8Item
        {
        public:
            virtual void OnDraw(CDC* pDC, int iIndex, const CRect& rtRect) = NULL;
        };
    private:
        struct ItemData_st
        {
            CI8IconItem* pclIconItem;
        };
    private:
        DECLARE_DYNAMIC(CI8IconView)
        DECLARE_MESSAGE_MAP()
    private:
        BOOL m_bCreate;
        CImageList m_clImageList;
        CFont m_clFont;
        int m_iIconHoriSpace;
        CI8SkinBitmap m_clBGBitmap;
        BOOL m_bBGBitmapStretch;
        UINT m_nBGColor;
    public:
        CI8IconView();
        void SetBackground(const stdex::tString& sImagePath, BOOL bStretch = TRUE);
        void SetBackground(UINT nBGColor);
        void SetSelected(int iIndex, BOOL bSelected);
        void SetItemSize(const CSize& clItemSize, const CSize& clItemSpace);
    protected:
        virtual void Init();
        afx_msg void OnDestroy();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnNcPaint();
        virtual void OnDeleteItem(int iIndex);
        int AddItem(CI8IconItem* pclItem);
        BOOL IsItemSelected(int iIndex) const;
    private:
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        virtual void PreSubclassWindow();
        afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
    };
}
