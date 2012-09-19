#pragma once

#include "I8SkinCommonFunction.h"
#include <afxhtml.h>

namespace I8SkinCtrl_ns
{
    static int WM_HTTP_ERROR_MESSAGE    = RegisterWindowMessage(TEXT("_i8desk_http_error_message"));

    class CI8HtmlView : public CHtmlView
    {
    	DECLARE_DYNAMIC(CI8HtmlView)
    public:
    	CI8HtmlView();
    	virtual ~CI8HtmlView();
    public:
    	void SetNoScroll();
    private:
    	BOOL m_bScroll;
    
    	//处理IE事件
    	virtual void	OnTitleChange(LPCTSTR lpszText);
    	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *info);
    	virtual void	OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
    	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
    		LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);	
    
    	//过滤掉Doc/View的一些处理。
    	virtual void PostNcDestroy() {  }
    	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
    	afx_msg void OnDestroy();
    	DECLARE_MESSAGE_MAP();
    };
}
