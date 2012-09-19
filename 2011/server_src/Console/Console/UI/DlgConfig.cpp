// DlgConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgConfig.h"
#include "../ManagerInstance.h"
#include "UIHelper.h"
#include "../MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgConfig

IMPLEMENT_DYNCREATE(CDlgConfig, CDialog)

CDlgConfig::CDlgConfig(CWnd* pParent)
	: i8desk::ui::BaseWnd(CDlgConfig::IDD, pParent)
	, wndOptUserMgr_(this)
	, wndOptClientSet_(this)
	, wndOptDownloadSet_(this)
	, wndOptSafeSet_(this)
{

}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_ACOUNT, wndBtnAcount_);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_CLIENT, wndBtnClient_);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_DOWNLOAD, wndBtnDownload_);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_SAFE, wndBtnSafe_);
	DDX_Control(pDX, IDC_STATIC_CONFIG, wndStaConfig_);

	DDX_Control(pDX, IDC_BUTTON_CONFIG_CONSUME, wndBtnOK_);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_CANCEL, wndBtnCancel_);
}


BEGIN_EASYSIZE_MAP(CDlgConfig)   
	EASYSIZE(IDC_BUTTON_CONFIG_ACOUNT, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CONFIG_CLIENT, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CONFIG_DOWNLOAD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CONFIG, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_BUTTON_CONFIG_CONSUME, ES_BORDER, IDC_STATIC_CONFIG, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CONFIG_CANCEL, ES_BORDER, IDC_STATIC_CONFIG, ES_KEEPSIZE, ES_KEEPSIZE, 0)
END_EASYSIZE_MAP 


BEGIN_MESSAGE_MAP(CDlgConfig, i8desk::ui::BaseWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_ACOUNT, &CDlgConfig::OnBnClickedButtonConfigAcount)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_CLIENT, &CDlgConfig::OnBnClickedButtonConfigClient)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_DOWNLOAD, &CDlgConfig::OnBnClickedButtonConfigDownload)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_SAFE, &CDlgConfig::OnBnClickedButtonConfigSafe)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_CONSUME, &CDlgConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG_CANCEL, &CDlgConfig::OnBnClickedCancel)
	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgConfig::OnApplySelect)
END_MESSAGE_MAP()




void CDlgConfig::Register()
{

}

void CDlgConfig::UnRegister()
{

}

void CDlgConfig::OnRealDataUpdate()
{

}

void CDlgConfig::OnReConnect()
{

}

void CDlgConfig::OnAsyncData()
{

}

void CDlgConfig::OnDataComplate()
{

}


BOOL CDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	// WorkLine

	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));


	// 初始化Button
	HBITMAP acount[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/AcountConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/AcountConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/AcountConfig_Click.png"))
	};
	wndBtnAcount_.SetImages(acount[0], acount[1], acount[2]);

	HBITMAP update[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/UpdateConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/UpdateConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/UpdateConfig_Click.png"))
	};
	wndBtnDownload_.SetImages(update[0], update[1], update[2]);

	HBITMAP client[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/ClientConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/ClientConfig.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/ClientConfig_Click.png"))
	};
	wndBtnClient_.SetImages(client[0], client[1], client[2]);
	HBITMAP safe[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SafeSet.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SafeSet.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SafeSet_Click.png"))
	};
	wndBtnSafe_.SetImages(safe[0], safe[1], safe[2]);

	HBITMAP ok[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnConfirm.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnConfirm_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnConfirm_Press.png"))
	};
	wndBtnOK_.SetImages(ok[0], ok[1], ok[2]);

	HBITMAP cancel[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnCancel.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnCancel_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Config/SystemSet_BtnCancel_Press.png"))
	};
	wndBtnCancel_.SetImages(cancel[0], cancel[1], cancel[2]);

	wndBtnAcount_.SetCheck(BST_CHECKED);


	// 初始化OptUserMgr Dlg
	wndOptUserMgr_.Create(CDlgCfgUserMgr::IDD, this);
	wndOptClientSet_.Create(CDlgCfgClientSet::IDD, this);
	wndOptDownloadSet_.Create(CDlgCfgDownloadSet::IDD, this);
	wndOptSafeSet_.Create(CDlgCfgSafeCenter::IDD, this);


	wndMgr_.Register(CDlgCfgUserMgr::IDD, &wndOptUserMgr_);
	wndMgr_.Register(CDlgCfgClientSet::IDD, &wndOptClientSet_);
	wndMgr_.Register(CDlgCfgDownloadSet::IDD, &wndOptDownloadSet_);
	wndMgr_.Register(CDlgCfgSafeCenter::IDD, &wndOptSafeSet_);

	wndMgr_.SetDestWindow(GetDlgItem(IDC_STATIC_CONFIG));
	
	wndMgr_[CDlgCfgUserMgr::IDD];

	return TRUE;  // return TRUE unless you set the focus to a control
}


// CDlgConfig 消息处理程序
void CDlgConfig::OnDestroy()
{
	CDialog::OnDestroy();

}

void CDlgConfig::OnPaint()
{
	//__super::OnPaint();

	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	CRect rcWork;
	wndOptUserMgr_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, &workOutLine_);
}

void CDlgConfig::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

	wndMgr_.UpdateSize();
	if( !::IsWindow(wndOptClientSet_.GetSafeHwnd()) )
		return;

	wndOptClientSet_.Invalidate();
	wndOptDownloadSet_.Invalidate();
	wndOptUserMgr_.Invalidate();
	wndOptSafeSet_.Invalidate();

	wndBtnOK_.Invalidate();
	wndBtnCancel_.Invalidate();
}

void CDlgConfig::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
}


void CDlgConfig::OnBnClickedButtonConfigAcount()
{
	wndMgr_[CDlgCfgUserMgr::IDD];
	wndOptUserMgr_.ReadData();
}

void CDlgConfig::OnBnClickedButtonConfigClient()
{
	//if( !::IsWindow(wndOptClientSet_.GetSafeHwnd()) )
	//{
	//	wndOptClientSet_.Create(CDlgCfgClientSet::IDD, this);
	//	wndMgr_.Register(CDlgCfgClientSet::IDD, &wndOptClientSet_);
	//}

	wndMgr_[CDlgCfgClientSet::IDD];
	wndOptClientSet_.ReadData();
}

void CDlgConfig::OnBnClickedButtonConfigDownload()
{
	wndMgr_[CDlgCfgDownloadSet::IDD];
	wndOptDownloadSet_.ReadData();
}

void CDlgConfig::OnBnClickedButtonConfigSafe()
{
	wndMgr_[CDlgCfgSafeCenter::IDD];
	wndOptSafeSet_.ReadData();
}

void CDlgConfig::OnBnClickedOk()
{
	if(wndMgr_.GetCurrentControl()->SendMessage(WM_MSG_OK))
	{
		CMessageBox msgDlg(_T("提示"), _T("系统成功设置"));
		msgDlg.DoModal();
		return ;
	}
}

void CDlgConfig::OnBnClickedCancel()
{

}

LRESULT CDlgConfig::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 1 )
	{
		wndMgr_[CDlgCfgUserMgr::IDD];
		wndOptUserMgr_.ReadData();
	}
	else if( wParam == 2 )
	{
		wndMgr_[CDlgCfgClientSet::IDD];
		wndOptClientSet_.ReadData();
	}	
	else
	{
		wndMgr_[CDlgCfgDownloadSet::IDD];
		wndOptDownloadSet_.ReadData();	
		wndBtnAcount_.SetCheck(BST_UNCHECKED);
		wndBtnClient_.SetCheck(BST_UNCHECKED);
		wndBtnDownload_.SetCheck(BST_CHECKED);
	}

	return TRUE;
}
