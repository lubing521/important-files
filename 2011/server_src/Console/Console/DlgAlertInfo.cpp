// DlgAlertInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DlgAlertInfo.h"

#include "ManagerInstance.h"

// CDlgAlertInfo 对话框


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



enum { WM_QUIT_ALERT = WM_USER + 0x100 };


IMPLEMENT_DYNCREATE(CDlgAlertInfo, CDHtmlDialog)

CDlgAlertInfo::CDlgAlertInfo()
{

}

CDlgAlertInfo::~CDlgAlertInfo()
{
}

void CDlgAlertInfo::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAlertInfo, CDHtmlDialog)
	ON_MESSAGE(WM_QUIT_ALERT, &CDlgAlertInfo::OnDestroyWindow)
END_MESSAGE_MAP()



// CDlgAlertInfo 消息处理程序

BOOL CDlgAlertInfo::OnInitDialog()
{
	__super::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	int cx	= ::GetSystemMetrics(SM_CXSCREEN); 
	int cy	= ::GetSystemMetrics(SM_CYSCREEN);

	CRect rcWindow;
	GetWindowRect(rcWindow);

	SetWindowPos(&wndTopMost, cx - rcWindow.Width(), cy - rcWindow.Height(), 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	Navigate(_T("http://ads.i8.com.cn/ad/client/fud_new2011.html"));
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgAlertInfo::OnCancel()
{
	DestroyWindow();
}

void CDlgAlertInfo::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}


LRESULT CDlgAlertInfo::OnDestroyWindow(WPARAM wParam, LPARAM lParam)
{
	OnCancel();
	return 0;
}

struct AlertHelper
{
	CDlgAlertInfo *alert_;

	AlertHelper()
		: alert_(0)
	{}
	~AlertHelper()
	{
		/*if( alert_ != 0 )
			delete alert_;*/
	}

	void Start();
	void Stop();
};

void AlertHelper::Start()
{
	if( alert_ != 0 )
	{
		if( ::IsWindow(alert_->GetSafeHwnd()) )
		{
			alert_->PostMessage(WM_QUIT_ALERT);
		}
		alert_ = 0;
	}

	alert_ = new CDlgAlertInfo;
	alert_->Create(CDlgAlertInfo::IDD, alert_->GetDesktopWindow());
}

void AlertHelper::Stop()
{

}

void AlertDlg()
{
	static AlertHelper alert;
	alert.Start();
}