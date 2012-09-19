#pragma once

namespace I8SkinCtrl_ns
{
    class CI8Edit : public CEdit
    {
    	DECLARE_DYNAMIC(CI8Edit)
    public:
    	CI8Edit();
    	virtual ~CI8Edit();
    public:
    	void SetDefaultText(LPCTSTR lpszDef) { m_strDef = lpszDef; }
    	CString GetDefaultText() { return m_strDef; }
    protected:
    	void xProcess();
    	virtual BOOL PreTranslateMessage(MSG* pMsg);
    	virtual void PreSubclassWindow();
    	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    	afx_msg void OnSetFocus(CWnd* pOldWnd);
    	afx_msg void OnKillFocus(CWnd* pNewWnd);
    	DECLARE_MESSAGE_MAP()
    private:
    	CString m_strDef;
    };
}
