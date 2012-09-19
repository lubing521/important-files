#pragma once

#include "I8LabelWnd.h"
#include <list>

namespace I8SkinCtrl_ns
{
    class CI8BaseTooltip : public CI8LabelWnd
    {
        DECLARE_MESSAGE_MAP()
	    DECLARE_DYNAMIC(CI8BaseTooltip)
    private:
        enum State_en
        {
            E_ST_CLOSE = 0,
            E_ST_WAIT_START,
            E_ST_DISPLAY,
            E_ST_WAIT_CLOSE,
            E_ST_CLOSING
        };
    private:
        State_en m_enState;
        BOOL m_bEnableTooltip;
        int m_iStartDelayTime; //启动延时
        int m_iDelayTime; //关闭延时
        std::list<std::pair<CRect, UINT>> m_clArea; //出现提示框的区域
        const std::pair<CRect, UINT>* m_pCurArea; //当前区域
        HWND m_hMSGWnd;
        CPoint m_clLastCursorPos;
        int m_iAlpha;
        int m_iDisappearTime;
    public:
	    CI8BaseTooltip();
        virtual ~CI8BaseTooltip();
        void EnableTooltip(BOOL bEnable = TRUE);
        void SetStartDelayTime(int iStartDelayTime);
        void SetDelayTime(int iDelayTime);
        void SetDisappearTime(int iDisappearTime);
        void SetWnd(HWND hWnd);
        void AddArea(const CRect& rtRect, UINT uiInfo = 0);
        void ClearArea();
        void RelayEvent(const MSG* pMSG);
    protected:
        afx_msg void OnTimer(UINT_PTR nIDEvent);
        int GetCurAreaIndex() const;
        UINT GetCurAreaInfo() const;
        virtual void CalculateClientSize(CSize& clClientSize);
        BOOL IsMouseInTooltip() const;
        BOOL IsMouseInArea(HWND hWnd) const;
        BOOL IsMouseInCurArea() const;
        void CloseTooltipWnd();
    private:
        static void CALLBACK EXPORT WaitTimeProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);
        void UpdateCurArea();
        void CreateTooltipWnd();
        CPoint GetDesPoint() const;
    };
}


