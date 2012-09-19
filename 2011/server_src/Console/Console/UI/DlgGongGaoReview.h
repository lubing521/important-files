#pragma once
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinImage.h"


// CDlgGongGaoReview 对话框

class CDlgGongGaoReview : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgGongGaoReview)

public:
	CDlgGongGaoReview(LPCTSTR gonggao, LPCTSTR front,  CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGongGaoReview();

// 对话框数据
	enum { IDD = IDD_DIALOG_GONGGAO_REVIEW };
private:
	LOGFONT		logfont_;
	CString		strFontString_;
	CString		textGongGao_;
	CFont		font_;
	COLORREF	fontColor_;

	i8desk::ui::SkinLabel     wndLabelGongGao_;

	CEdit		wndEditGongGao_;
	ATL::CImage outboxLine_;

private:
	void SetAdvFont();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
