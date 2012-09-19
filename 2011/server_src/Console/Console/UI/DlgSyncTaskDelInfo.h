#pragma once
#include "afxwin.h"


#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinRadioBox.h"
#include "../../../../include/Extend STL/UnicodeStl.h"

#include <vector>
// CDlgSyncTaskDelInfo 对话框

class CDlgSyncTaskDelInfo : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgSyncTaskDelInfo)

public:
	CDlgSyncTaskDelInfo(const std::vector<long> &games, const std::vector<stdex::tString> &Sids, bool viewstatus, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSyncTaskDelInfo();

	// 对话框数据
	enum { IDD = IDD_DIALOG_SYNCTASK_DELINFO };

private:
	i8desk::ui::SkinRadioBox wndRadioDelSyncAll_;
	i8desk::ui::SkinRadioBox wndRadioDelSyncRecord_;

	i8desk::ui::SkinLabel wndLabelAll_;
	i8desk::ui::SkinLabel wndLabelOne_;

	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;
	const  std::vector<long> &curgames_;
	const  std::vector<stdex::tString> &curSids_;

	bool ViewStatus_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();


};

