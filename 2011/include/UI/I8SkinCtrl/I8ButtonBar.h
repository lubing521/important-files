#pragma once

#include "I8Button.h"
#include "I8BGImage.h"
#include "../../Extend STL/UnicodeStl.h"
#include <vector>

namespace I8SkinCtrl_ns
{
    class CI8ButtonBar : public CWnd
    {
        DECLARE_DYNAMIC(CI8ButtonBar)
        DECLARE_MESSAGE_MAP()
    public:
        struct ButtonInfo_st
        {
            UINT nID;
            stdex::tString sText;
            stdex::tString sSkinName;
            CSize szButtonSize;
        };
        const static CSize C_BUTTON_SIZE_IMAGE;
        const static CSize C_BUTTON_SIZE_NULL;
    private:
        CBrush m_clParentBGBrush;
        CI8BGImage m_clBGImage;
        CI8BGCache m_clBGCache;
        CI8Trusteeship<CI8Button> m_clButton;
        int m_iButtonHoriSpace;
        int m_iLeftSpace;
        int m_iRightSpace;
        int m_iTopSpace;
    public:
        CI8ButtonBar();
        virtual ~CI8ButtonBar();
        void Create(DWORD dwStyle, const CRect& rtRect, CWnd* pclWnd, const CSize& szSize, const ButtonInfo_st* pstButtonInfo, int iCount, const stdex::tString& sBaseDir);
        template<int iCount> inline void Create(DWORD dwStyle, const CRect& rtRect, CWnd* pclWnd, const CSize& szSize, const ButtonInfo_st(&astButtonInfo)[iCount], const stdex::tString& sBaseDir){Create(dwStyle, rtRect, pclWnd, szSize, astButtonInfo, iCount, sBaseDir);}
        void SetHoriSpace(int iButtonHoriSpace);
        CI8Button& GetButton(UINT nID);
        void SetBackground(const stdex::tString& sSkinName);
        void SetHoriEdge(int iLeftSpace, int iRightSpace);
        void SetTopEdge(int iTopSpace);
        CSize GetSize(const CSize& szSize, const ButtonInfo_st* pstButtonInfo, int iCount);
        template<int iCount> inline CSize GetSize(const CSize& szSize, const ButtonInfo_st* pstButtonInfo, int iCount){return GetSize(szSize, astButtonInfo, iCount);}
    protected:
        void AdjustSize();
        virtual afx_msg void OnButtonClick(UINT nID);
    private:
        afx_msg void OnSize(IN UINT nType, IN int cx, IN int cy);
        afx_msg void OnMove(int x, int y);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
    };
}
