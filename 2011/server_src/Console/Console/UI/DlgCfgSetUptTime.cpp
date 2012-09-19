// SetUptTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgCfgSetUptTime.h"
#include "../MessageBox.h"
#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../ManagerInstance.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDlgCfgSetUptTime, CNonFrameChildDlg)

CDlgCfgSetUptTime::CDlgCfgSetUptTime(CWnd* pParent /*=NULL*/)
: CNonFrameChildDlg(CDlgCfgSetUptTime::IDD, pParent)
{

}

CDlgCfgSetUptTime::~CDlgCfgSetUptTime()
{
}

void CDlgCfgSetUptTime::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_SETTIME_STARTTIME, wnddateStart_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_SETTIME_STOPTIME, wnddateEnd_);
	DDX_Control(pDX, IDC_STATIC_SETTIME_STARTTIME, wndStartTime_);
	DDX_Control(pDX, IDC_STATIC_SETTIME_STOPTIME, wndStopTime_);
	DDX_Control(pDX, IDC_BUTTON_SETTIME_OK, wndbtnOk_);
	DDX_Control(pDX, IDC_BUTTON_SETTIME_CANCEL, wndbtnCancel_);

}

BEGIN_MESSAGE_MAP(CDlgCfgSetUptTime, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_SETTIME_OK, &CDlgCfgSetUptTime::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SETTIME_CANCEL, &CDlgCfgSetUptTime::OnBnClickedCancel)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

BOOL CDlgCfgSetUptTime::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("起始时间设置"));
	wnddateStart_.SetTime(&CTime(2009, 1, 1, 21, 0, 0));
	wnddateEnd_.SetTime(&CTime(2009, 1, 1, 23, 59, 0));
	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	wndStartTime_.SetThemeParent(GetSafeHwnd());
	wndStopTime_.SetThemeParent(GetSafeHwnd());

	return TRUE;
}

void CDlgCfgSetUptTime::OnBnClickedOk()
{
	wnddateStart_.GetTime(tmStart);
	wnddateEnd_.GetTime(tmEnd);

	tmStart = CTime(2009, 1, 1, tmStart.GetHour(), tmStart.GetMinute(), 0);
	tmEnd   = CTime(2009, 1, 1, tmEnd.GetHour(),   tmEnd.GetMinute(),   0);
	if (tmStart >= tmEnd)
	{
		CMessageBox msgDlg(_T("提示"),_T("请保证结束时间大于开始时间。"));
		msgDlg.DoModal();

		return ;
	}

	OnOK();
}

void CDlgCfgSetUptTime::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CDlgCfgSetUptTime::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wnddateStart_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wnddateEnd_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}