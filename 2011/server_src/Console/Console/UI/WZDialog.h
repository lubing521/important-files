#pragma once
#include "WindowManager.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinImage.h"
#include "Skin/SkinButton.h"

// CWZDialog 对话框

class CWZDialog 
	: public CNonFrameChildDlg
	, public i8desk::ui::ChildMgr
{
	DECLARE_DYNAMIC(CWZDialog)

public:
	CWZDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_WIZARD_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	i8desk::ui::SkinImage wndLogo_;
	CListBox wndListSel_;

	i8desk::ui::SkinButton wndBtnPrev_;
	i8desk::ui::SkinButton wndBtnNext_;
	i8desk::ui::SkinButton wndBtnOK_;
	i8desk::ui::SkinButton wndBtnCancel_;

	size_t curWnd_;							// 当前向导所选项
	typedef std::tr1::shared_ptr<CDialog> DialogPtr;
	typedef std::vector<DialogPtr> WZWnds;	// 向导所有项
	WZWnds wzWnds_;
	 
	ATL::CImage workOutLine_;

private:
	template < typename T >
	void _AddProperty(const CRect &rc, const stdex::tString &title);
	void _ShowProperty(size_t index);


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnPaint();

	afx_msg void OnBnClickedButtonWzPrev();
	afx_msg void OnBnClickedButtonWzNext();
	afx_msg void OnBnClickedButtonWzOk();
	afx_msg void OnBnClickedButtonWzCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	

	afx_msg void OnLbnSelchangeListSelect();
};
