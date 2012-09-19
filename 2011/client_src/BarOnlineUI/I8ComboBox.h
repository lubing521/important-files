#pragma once

#include "I8Edit.h"

namespace I8SkinCtrl_ns
{
    class CI8ComboBox : public CComboBox
    {
    	DECLARE_DYNAMIC(CI8ComboBox)
    public:
    	CI8ComboBox();
    	virtual ~CI8ComboBox();
    public:
    	CString GetText()
    	{
    		CString str;
    		if (m_Edit.GetSafeHwnd() != NULL)
    			m_Edit.GetWindowText(str);
    
    		return str;
    	}
    protected:
    	afx_msg void OnPaint();
    	afx_msg void OnSize(UINT nType, int cx, int cy);
    	afx_msg void OnDestroy();
    	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    	afx_msg void OnSetFocus(CWnd* pOldWnd);
    	DECLARE_MESSAGE_MAP()
    private:
    	CBitmap* m_pbkImage;
    	CBitmap* m_pRight;
    	CI8Edit  m_Edit;
    };
}
