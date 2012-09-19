#pragma once

#include "I8BGImage.h"
#include "I8BGCache.h"
#include <list>
#include "I8CtrlBackground.h"

namespace I8SkinCtrl_ns
{
    class CI8ItemList : public CWnd
    {
    public:
        struct ItemInfo_st
        {
            CI8ItemList* pclWnd;
            int iIndex;
            CRect rtRect;
            BOOL bSelected;
            BOOL bHover;
        };
        class CI8Item
        {
        public:
            inline CI8Item(){}
            inline virtual ~CI8Item(){}
            virtual void OnDraw(CDC* pDC, const ItemInfo_st& stItemInfo) = NULL;
            virtual void OnLButtonDown(UINT nFlags, CPoint point, const ItemInfo_st& stItemInfo){}
            virtual void OnLButtonUp(UINT nFlags, CPoint point, const ItemInfo_st& stItemInfo){}
            virtual void OnRButtonDown(UINT nFlags, CPoint point, const ItemInfo_st& stItemInfo){}
            virtual void OnRButtonUp(UINT nFlags, CPoint point, const ItemInfo_st& stItemInfo){}
            virtual void OnLButtonDblClk(UINT nFlags, CPoint point, const ItemInfo_st& stItemInfo){}
            inline virtual void OnSelect(const ItemInfo_st& stItemInfo){}
            inline virtual void OnUnselect(const ItemInfo_st& stItemInfo){}
            inline virtual void OnAdd(const ItemInfo_st& stItemInfo){}
            inline virtual void OnDelete(const ItemInfo_st& stItemInfo){}
            inline virtual void OnDeleted(const ItemInfo_st& stItemInfo){}
            inline virtual CSize GetItemSize(int iIndex) const{assert(0 && "没有实现GetItemSize函数"); return CSize(0, 0);}
        };
        class CI8CalculateItemRect
        {
        public:
            CI8CalculateItemRect(){}
            virtual ~CI8CalculateItemRect(){}
            virtual void Begin(const CRect& rtClient) = NULL;
            virtual void CalculateItem(int iIndex, CI8Item* pclItem, CRect& rtItem, BOOL& bVisible) = NULL;
            virtual void End(){}
        };
    private:
        struct ItemData_st
        {
            CI8Item* pclItem;
            CRect rtItem;
            int iIndex;
            BOOL bVisible;
            BOOL bSelected;
        };
    private:
        DECLARE_DYNAMIC(CI8ItemList)
        DECLARE_MESSAGE_MAP()
    private:
        CI8CtrlBackground* m_pclBackground;
        CI8BGCache m_clBGCache;
        BOOL m_bMouseHover;
        std::list<ItemData_st> m_clItems;
        std::list<std::list<ItemData_st>::iterator> m_clVisibleItems;
        std::list<std::list<ItemData_st>::iterator> m_clSelectedItems;
        std::list<ItemData_st>::iterator m_clCurItem;
        BOOL m_bMulSelect;
        BOOL m_bDrawReverse;
        BOOL m_bAutoFreeItem;
        CI8CalculateItemRect& m_clCalculateItemRect;
    public:
        CI8ItemList(CI8CalculateItemRect& clCalculateItemRect, BOOL bAutoFreeItem, CI8CtrlBackground* pclBackground = NULL);
        virtual ~CI8ItemList();
        void Create(DWORD dwStyle, const CRect& rtRect, CWnd* pclWnd, UINT nID = 0);
        void SetSelected(int iIndex, BOOL bSelected, BOOL bNotify = TRUE);
        void Select(int iIndex, BOOL bNotify = TRUE);
        void CancelSelect();
        void Clear();
        void AddItem(CI8Item* pclItem, BOOL bUpdate = TRUE);
        void SetItemCount(int iCount, CI8Item* pclItem);
        void DeleteItem(int iIndex, BOOL bUpdate = TRUE);
        BOOL IsSelected(int iIndex) const;
        BOOL IsSelected() const;
        int GetSelected() const;
        int GetItemCount() const;
        int GetSelectedItemCount() const;
        CRect GetClientRect() const;
        CI8Item* GetItem(int iIndex);
        void SetDrawReverse(BOOL bDrawReverse = TRUE);
    protected:
        virtual void Init();
        afx_msg void OnDestroy();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnSize(IN UINT nType, IN int cx, IN int cy);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnMouseLeave(void);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
        void UpdateItemRect(BOOL bRedraw = TRUE);
        void NeedRedraw();
        virtual CRect GetItemArea(const CRect& rtRect) const;
        virtual void OnUpdateItemRect();
    private:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        std::list<ItemData_st>::iterator GetItemByPos(const CPoint& clPos);
        void CancelSelect(BOOL bNotify);
        void SetSelected(const std::list<ItemData_st>::iterator& clItem, BOOL bSelected, BOOL bNotify = TRUE);
        std::list<ItemData_st>::iterator GetItemFromIndex(int iIndex);
        void DrawAllItem(CDC* pDC);
    };
}
