// DlgMonitor.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgMonitor.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/utility.h"
#include "UIHelper.h"
#include "../Misc.h"
#include "../Business/MonitorBusiness.h"
#include "../LogHelper.h"
#include "../AsyncDataHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



enum { WM_LOG_MSG = WM_USER + 0x111, WM_EXCEPT_MSG = WM_USER + 0x112 };

// CDlgMonitor

IMPLEMENT_DYNCREATE(CDlgMonitor, CDialog)

CDlgMonitor::CDlgMonitor(CWnd* pParent)
	: i8desk::ui::BaseWnd(CDlgMonitor::IDD, pParent)
	, outLine_(new ATL::CImage)
	, isDebug_(false)
{

}

CDlgMonitor::~CDlgMonitor()
{
}

void CDlgMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MONITOR_SERVER,		wndLabelServer_);
	DDX_Control(pDX, IDC_STATIC_MONITOR_DOWNLOAD,	wndLabelDownload_);
	DDX_Control(pDX, IDC_STATIC_MONITOR_UPDATE,		wndLabelUpdate_);
	DDX_Control(pDX, IDC_STATIC_MONITOR_SYNCTASK,	wndLabelSyncTask_);
	DDX_Control(pDX, IDC_STATIC_MONITOR_LOG,		wndLabelLog_);
	DDX_Control(pDX, IDC_STATIC_MONITOR_CLIENT_MONITOR, wndLabelCliMonitor_);

	DDX_Control(pDX, IDC_BUTTON_MONITOR_SERVER,		wndBtnServer_);
	DDX_Control(pDX, IDC_BUTTON_MONITOR_DOWNLOAD,	wndBtnDownload_);
	DDX_Control(pDX, IDC_BUTTON_MONITOR_UPDATE,		wndBtnUpdate_);
	DDX_Control(pDX, IDC_BUTTON_MONITOR_SYNCTASK,	wndBtnSyncTask_);
	DDX_Control(pDX, IDC_BUTTON_MONITOR_CLIENT_EXCEPTION, wndBtnCliExcept_);

	DDX_Control(pDX, IDC_LIST_MONITOR_SERVER,		wndListServer_);
	DDX_Control(pDX, IDC_LIST_MONITOR_DOWNLOAD,		wndListDownload_);
	DDX_Control(pDX, IDC_LIST_MONITOR_UPDATE,		wndListUpdate_);
	DDX_Control(pDX, IDC_LIST_MONITOR_SYNCTASK,		wndListSyncTask_);
	DDX_Control(pDX, IDC_LIST_MONITOR_LOG,			wndListLog_);
	DDX_Control(pDX, IDC_LIST_MONITOR_CLIENT_EXCEPTION, wndListCliExcept_);

	DDX_Control(pDX, IDC_STATIC_MONITOR_LABEL,		wndLabel_); 
	DDX_Control(pDX, IDC_STATIC_MONITOR_LEFT,		wndLabelLeft_); 
}

BEGIN_EASYSIZE_MAP(CDlgMonitor)   
	EASYSIZE(IDC_STATIC_MONITOR_LABEL, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_HCENTER)
	EASYSIZE(IDC_STATIC_MONITOR_LEFT, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, ES_VCENTER)

	EASYSIZE(IDC_LIST_MONITOR_SERVER, ES_BORDER, IDC_STATIC_MONITOR_LABEL, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_MONITOR_DOWNLOAD, ES_BORDER, IDC_LIST_MONITOR_SERVER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_MONITOR_SYNCTASK, IDC_STATIC_MONITOR_LABEL, IDC_LIST_MONITOR_DOWNLOAD, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_MONITOR_UPDATE, ES_BORDER, IDC_LIST_MONITOR_DOWNLOAD, IDC_LIST_MONITOR_SYNCTASK, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_MONITOR_LOG, ES_BORDER, IDC_LIST_MONITOR_SYNCTASK, IDC_LIST_MONITOR_SYNCTASK, ES_BORDER, 0)
	EASYSIZE(IDC_LIST_MONITOR_CLIENT_EXCEPTION, IDC_LIST_MONITOR_LOG, IDC_LIST_MONITOR_UPDATE, ES_BORDER, ES_BORDER, 0)

	EASYSIZE(IDC_BUTTON_MONITOR_SERVER, IDC_LIST_MONITOR_SERVER, IDC_STATIC_MONITOR_LABEL, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_MONITOR_DOWNLOAD, IDC_LIST_MONITOR_DOWNLOAD, IDC_LIST_MONITOR_SERVER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_MONITOR_UPDATE, IDC_LIST_MONITOR_UPDATE, IDC_LIST_MONITOR_DOWNLOAD, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_MONITOR_SYNCTASK, IDC_LIST_MONITOR_SYNCTASK, IDC_LIST_MONITOR_DOWNLOAD, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_MONITOR_CLIENT_EXCEPTION, IDC_LIST_MONITOR_CLIENT_EXCEPTION, IDC_LIST_MONITOR_SYNCTASK, ES_BORDER, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_STATIC_MONITOR_SERVER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_MONITOR_DOWNLOAD, ES_BORDER, IDC_LIST_MONITOR_SERVER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_MONITOR_UPDATE, ES_BORDER, IDC_LIST_MONITOR_DOWNLOAD, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_MONITOR_SYNCTASK, IDC_STATIC_MONITOR_LABEL, IDC_LIST_MONITOR_DOWNLOAD, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_MONITOR_LOG, ES_BORDER, IDC_LIST_MONITOR_DOWNLOAD, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_MONITOR_CLIENT_MONITOR, IDC_STATIC_MONITOR_LABEL, IDC_LIST_MONITOR_DOWNLOAD, ES_KEEPSIZE, ES_KEEPSIZE, 0)

END_EASYSIZE_MAP 


BEGIN_MESSAGE_MAP(CDlgMonitor, i8desk::ui::BaseWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()

	ON_BN_CLICKED(IDC_BUTTON_MONITOR_SERVER, &CDlgMonitor::OnBnClickedButtonMonitorServer)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_DOWNLOAD, &CDlgMonitor::OnBnClickedButtonMonitorDownload)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_UPDATE, &CDlgMonitor::OnBnClickedButtonMonitorUpdate)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_SYNCTASK, &CDlgMonitor::OnBnClickedButtonMonitorSynctask)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_CLIENT_EXCEPTION, &CDlgMonitor::OnBnClickedButtonMonitorExcept)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_SERVER, &CDlgMonitor::OnLvnGetdispinfoListServer)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_DOWNLOAD, &CDlgMonitor::OnLvnGetdispinfoListDownload)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_UPDATE, &CDlgMonitor::OnLvnGetdispinfoListUpdate)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_SYNCTASK, &CDlgMonitor::OnLvnGetdispinfoListSyncTask)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_LOG, &CDlgMonitor::OnLvnGetdispinfoListLog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MONITOR_CLIENT_EXCEPTION, &CDlgMonitor::OnLvnGetdispinfoListExcept)

	ON_NOTIFY(NM_CLICK, IDC_LIST_MONITOR_DOWNLOAD, &CDlgMonitor::ONClickDownload)

	ON_MESSAGE(WM_LOG_MSG, &CDlgMonitor::OnLogMsg)
	ON_MESSAGE(WM_EXCEPT_MSG, &CDlgMonitor::OnExceptMsg)
	ON_MESSAGE(i8desk::ui::WM_CHANGE_DEBUG_MODE, &CDlgMonitor::OnChangeDebugMode)
	ON_MESSAGE(i8desk::ui::WM_SAVE_HARDWARE_MSG, &CDlgMonitor::OnSaveHardwareMsg)
END_MESSAGE_MAP()



namespace
{
	CCustomColumnTraitServerStatus *svrStatus = 0;
	CCustomColumnTraitServerStatus *GetServerStatus()
	{
		svrStatus = new CCustomColumnTraitServerStatus;
		svrStatus->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
		svrStatus->SetStatusImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/ServerOnline.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/ServerOffline.png")));
	
		return svrStatus;
	}


	CCustomColumnTrait *downloadStatus = 0;
	CCustomColumnTrait *GetDownloadStatus()
	{
		downloadStatus = new CCustomColumnTrait;
		downloadStatus->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return downloadStatus;
	}

	CCustomColumnTraitDownloadRes *downloadRes = 0;
	CCustomColumnTraitDownloadRes *GetDownloadRes()
	{
		downloadRes = new CCustomColumnTraitDownloadRes;
		downloadRes->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
		downloadRes->SetOperateImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/DonwloadOperate.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/DownloadOperateBK.png")));

		return downloadRes;
	}

	CCustomColumnTrait *GetTextTrait()
	{
		CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
		rowTrait->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return rowTrait;
	}

	
	HBITMAP* GetScroll()
	{
		static HBITMAP scroll[] = 
		{

			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Top.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Btm.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Top.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Btm.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/VScroll_Bar_Mid.png"))

		};

		return scroll;
	}
	

	enum { MASK_SERVER_NAME = 1,  MASK_SERVER_STATUS, MASK_SERVER_CLICONNECT,  MASK_SERVER_CONNECT, MASK_SERVER_SPEED, MASK_SERVER_BYTES };
	template < typename ListT >
	void InitServerList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(

			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		list.InsertColumnTrait(MASK_SERVER_NAME,	_T("服务器"),			LVCFMT_LEFT, 150, MASK_SERVER_NAME,		GetTextTrait());
		list.InsertColumnTrait(MASK_SERVER_STATUS,	_T("虚拟盘状态"),				LVCFMT_LEFT, 150, MASK_SERVER_STATUS,	GetServerStatus());
		list.InsertColumnTrait(MASK_SERVER_CLICONNECT,_T("客户机负载数"),	LVCFMT_LEFT, 150, MASK_SERVER_CLICONNECT,	GetTextTrait());
		list.InsertColumnTrait(MASK_SERVER_CONNECT,	_T("虚拟盘负载数"),	LVCFMT_LEFT, 150, MASK_SERVER_CONNECT,	GetTextTrait());
		list.InsertColumnTrait(MASK_SERVER_SPEED,	_T("虚拟盘总速度"),	LVCFMT_LEFT, 150, MASK_SERVER_SPEED,	GetTextTrait());
		list.InsertColumnTrait(MASK_SERVER_BYTES,	_T("虚拟盘已发送数据"),LVCFMT_LEFT, 150, MASK_SERVER_BYTES,	GetTextTrait());
	}

	enum { MASK_DOWNLOAD_STATUS = 1, MASK_DOWNLOAD_DOWNLOADING, MASK_DOWNLOAD_SPEED, MASK_DOWNLOAD_WAITTING, 
		MASK_DOWNLOAD_CONNECT, MASK_DOWNLOAD_TEMP_SIZE, MASK_DOWNLOAD_LIMIT, MASK_DOWNLOAD_CPUUSAGE };
	template < typename ListT >
	void InitDownloadList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		list.InsertColumnTrait(MASK_DOWNLOAD_STATUS,		_T("三层更新状态"),	LVCFMT_LEFT, 150, MASK_DOWNLOAD_STATUS,		GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_DOWNLOADING,	_T("正在下载"),		LVCFMT_LEFT, 100, MASK_DOWNLOAD_DOWNLOADING,GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_SPEED,			_T("下载总速度"),		LVCFMT_LEFT, 100, MASK_DOWNLOAD_SPEED,		GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_WAITTING,		_T("排队资源"),		LVCFMT_LEFT, 100, MASK_DOWNLOAD_WAITTING,	GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_CONNECT,		_T("有效连接数"),		LVCFMT_LEFT, 100, MASK_DOWNLOAD_CONNECT,	GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_TEMP_SIZE,		_T("临时文件大小"),	LVCFMT_LEFT, 100, MASK_DOWNLOAD_TEMP_SIZE,	GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_LIMIT,			_T("下载限速"),		LVCFMT_LEFT, 100, MASK_DOWNLOAD_LIMIT,		GetDownloadRes());
		list.InsertColumnTrait(MASK_DOWNLOAD_CPUUSAGE,		_T("CPU利用率"),		LVCFMT_LEFT, 180, MASK_DOWNLOAD_CPUUSAGE,	GetDownloadRes());
	}

	enum { MASK_UPDATEGAME_CLIENTS = 1, MASK_UPDATEGAME_SPEED };
	template < typename ListT >
	void InitUpdateList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(

			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		list.InsertColumnTrait(MASK_UPDATEGAME_CLIENTS,	_T("内网更新客户机数"),		LVCFMT_LEFT, 200, MASK_UPDATEGAME_CLIENTS,	GetTextTrait());
		list.InsertColumnTrait(MASK_UPDATEGAME_SPEED,	_T("更新总速度"),			LVCFMT_LEFT, 250, MASK_UPDATEGAME_SPEED,	GetTextTrait());
	}

	enum { MASK_SYNCTASK_CLIENTS = 1, MASK_SYNCTASK_SPEED };
	template < typename ListT >
	void InitSyncTaskList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		list.InsertColumnTrait(MASK_SYNCTASK_CLIENTS,	_T("同步任务总个数"),		LVCFMT_LEFT, 200, MASK_SYNCTASK_CLIENTS,	GetTextTrait());
		list.InsertColumnTrait(MASK_SYNCTASK_SPEED,		_T("同步总速度"),			LVCFMT_LEFT, 260, MASK_SYNCTASK_SPEED,		GetTextTrait());
	}

	enum { MASK_MODULE = 1, MASK_TIME, MASK_DESC };

	CCustomColumnTrait *logMsg = 0;
	template < typename ListT >
	void InitLogList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		logMsg = GetTextTrait();

		list.InsertColumnTrait(MASK_MODULE,	_T("模块"),		LVCFMT_LEFT, 100, MASK_MODULE,	GetTextTrait());
		list.InsertColumnTrait(MASK_TIME,	_T("时间"),		LVCFMT_LEFT, 100, MASK_TIME,	GetTextTrait());
		list.InsertColumnTrait(MASK_DESC,	_T("内容"),		LVCFMT_LEFT, 260, MASK_DESC,	logMsg);
	}

	CCustomColumnTrait *content				= 0;

	enum { MASK_EXCEPT_IP = 1, MASK_EXCEPT_NAME, MASK_EXCEPT_TIME, MASK_EXCEPT_CONTENT };

	template < typename ListT >
	void InitExceptList(ListT &list)
	{
		list.InsertHiddenLabelColumn();
		list.SetItemHeight(24);

		list.SetHeaderImage(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));
		list.SetScrollImage(GetScroll());

		list.InsertColumnTrait(MASK_EXCEPT_IP,		_T("客户机IP"),		LVCFMT_LEFT, 80, MASK_EXCEPT_IP,	GetTextTrait());
		list.InsertColumnTrait(MASK_EXCEPT_NAME,	_T("客户机名称"),		LVCFMT_LEFT, 100, MASK_EXCEPT_NAME,	GetTextTrait());
		list.InsertColumnTrait(MASK_EXCEPT_TIME,	_T("时间"),			LVCFMT_LEFT, 100, MASK_EXCEPT_TIME,	GetTextTrait());
		list.InsertColumnTrait(MASK_EXCEPT_CONTENT,	_T("内容"),			LVCFMT_LEFT, 150, MASK_EXCEPT_CONTENT,	content = GetTextTrait());
	}
}


void CDlgMonitor::Register()
{
	i8desk::GetRealDataMgr().Register(std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_MONITOR);
}

void CDlgMonitor::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_MONITOR);
}

void CDlgMonitor::OnRealDataUpdate()
{
	monitorStatus_ = *(i8desk::GetRealDataMgr().GetMonitorStatus());
	
	static size_t count = 0;
	static size_t diskcount = 0;

	if( count != monitorStatus_.ServersInfo.size() )
	{
		count = monitorStatus_.ServersInfo.size();
		wndListServer_.SetItemCount(count);
	}
	else
		wndListServer_.RedrawCurPageItems();


	wndListDownload_.SetItemCount(2);
	wndListUpdate_.SetItemCount(1);
	wndListSyncTask_.SetItemCount(1);


	struct AsyncDataImpl
	{
		static void Run()
		{
			i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
			i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetRunTypes());
		}
	};

	i8desk::AsyncDataHelper(std::tr1::bind(&AsyncDataImpl::Run));
}

void CDlgMonitor::OnReConnect()
{

}

void CDlgMonitor::OnAsyncData()
{
	
}

void CDlgMonitor::OnDataComplate()
{
}

void CDlgMonitor::_AddLog(const i8desk::LogRecordPtr &log)
{
	if( log == 0 )
		return;

	i8desk::business::monitor::PushLog(log);
	
	if( !::IsWindow(GetSafeHwnd()) )
		return;

	PostMessage(WM_LOG_MSG);
}

void CDlgMonitor::_AddExcept(const std::tr1::tuple<i8desk::db::TClientPtr, DWORD> &exceptInfo)
{
	i8desk::business::monitor::PushExcept(exceptInfo);

	if( !::IsWindow(GetSafeHwnd()) )
		return;

	PostMessage(WM_EXCEPT_MSG);
}

// CDlgMonitor 消息处理程序

BOOL CDlgMonitor::OnInitDialog()
{
	BaseWnd::OnInitDialog();

	i8desk::GetRecvDataMgr().RegisterLog(std::tr1::bind(&CDlgMonitor::_AddLog, this, std::tr1::placeholders::_1));
	i8desk::GetRecvDataMgr().RegisterCliExcept(std::tr1::bind(&CDlgMonitor::_AddExcept, this, std::tr1::placeholders::_1));

	static i8desk::LogHelper::Log2StorageType storage(std::tr1::bind(&CDlgMonitor::_AddLog, this, std::tr1::placeholders::_1));
	i8desk::Log().Add(storage);

	// Label

	wndLabelServer_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Server_Label.png")));
	wndLabelDownload_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Download_Label.png")));
	wndLabelUpdate_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Update_Label.png")));
	wndLabelSyncTask_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/SyncTask_Label.png")));
	wndLabelLog_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/ServerStatus_Label.png")));
	wndLabelCliMonitor_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/ClientMonitor_Label.png")));

	wndLabelServer_.SizeToFit();
	wndLabelDownload_.SizeToFit();
	wndLabelUpdate_.SizeToFit();
	wndLabelSyncTask_.SizeToFit();
	wndLabelLog_.SizeToFit();
	wndLabelCliMonitor_.SizeToFit();

	// List
	InitServerList(wndListServer_);
	InitDownloadList(wndListDownload_);
	InitUpdateList(wndListUpdate_);
	InitSyncTaskList(wndListSyncTask_);
	InitLogList(wndListLog_);
	InitExceptList(wndListCliExcept_);

	// Button
	HBITMAP info[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Btn_Details.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Btn_DetailsHover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Monitor/Btn_DetailsClick.png"))
	};
	wndBtnDownload_.SetImages(info[0], info[1], info[2]);
	wndBtnServer_.SetImages(info[0], info[1], info[2]);
	wndBtnSyncTask_.SetImages(info[0], info[1], info[2]);
	wndBtnUpdate_.SetImages(info[0], info[1], info[2]);
	wndBtnCliExcept_.SetImages(info[0], info[1], info[2]);

	// Work Area

	outLine_->Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));

	// 初始化图片
	HBITMAP rightArea[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Left_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Bottom_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Content_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Top_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Right_Line.png"))
	};
	for(size_t i = 0; i != _countof(rightArea); ++i)
	{
		area_[i].reset(new CImage);
		area_[i]->Attach(rightArea[i]);
	}

	INIT_EASYSIZE;

	wndListServer_.AutoAdjustWidth();
	wndListDownload_.AutoAdjustWidth();
	wndListUpdate_.AutoAdjustWidth();
	wndListSyncTask_.AutoAdjustWidth();
	wndListLog_.AutoAdjustWidth();
	wndListCliExcept_.AutoAdjustWidth();

	return TRUE;  // return TRUE unless you set the focus to a control
}




void CDlgMonitor::OnDestroy()
{
	// UnRegister
	i8desk::GetRecvDataMgr().RegisterLog(0);
	i8desk::GetRecvDataMgr().RegisterCliExcept(0);

}

LRESULT CDlgMonitor::OnChangeDebugMode(WPARAM, LPARAM)
{
	i8desk::business::monitor::ChangeDebugMode();
	size_t cnt = i8desk::business::monitor::GetLogCount();
	
	wndListLog_.SetItemCount(cnt);

	return TRUE;
}

LRESULT CDlgMonitor::OnSaveHardwareMsg(WPARAM, LPARAM)
{
	wndListCliExcept_.SetItemCount(i8desk::business::monitor::GetExceptCount());
	return TRUE;
}

LRESULT CDlgMonitor::OnLogMsg(WPARAM wParam, LPARAM lParam)
{
	wndListLog_.SetItemCount(i8desk::business::monitor::GetLogCount());
	return TRUE;
}

LRESULT CDlgMonitor::OnExceptMsg(WPARAM wParam, LPARAM lParam)
{
	wndListCliExcept_.SetItemCount(i8desk::business::monitor::GetExceptCount());
	return TRUE;
}

void CDlgMonitor::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	// List Frame
	CRect rcServer;
	wndListServer_.GetWindowRect(rcServer);
	
	CRect rcDownload;
	wndListDownload_.GetWindowRect(rcDownload);
	
	CRect rcUpdate;
	wndListUpdate_.GetWindowRect(rcUpdate);

	CRect rcSyncTask;
	wndListSyncTask_.GetWindowRect(rcSyncTask);
	
	CRect rcLog;
	wndListLog_.GetWindowRect(rcLog);

	CRect rcExcept;
	wndListCliExcept_.GetWindowRect(rcExcept);

	// Out Frame
	CRect rcServerTmp = rcServer;
	ScreenToClient(rcServerTmp);
	CRect rcServerFrame(rcServerTmp);
	rcServerFrame.DeflateRect(-10, -48, -10, -10);

	CRect rcDownloadTmp = rcDownload;
	ScreenToClient(rcDownloadTmp);
	CRect rcDownloadFrame(rcDownloadTmp);
	rcDownloadFrame.DeflateRect(-10, -48, -10, -10);

	CRect rcUpdateTmp = rcUpdate;
	ScreenToClient(rcUpdateTmp);
	CRect rcUpdateFrame(rcUpdateTmp);
	rcUpdateFrame.DeflateRect(-10, -48, -10, -10);

	CRect rcSyncTaskTmp = rcSyncTask;
	ScreenToClient(rcSyncTaskTmp);
	CRect rcSyncTaskFrame(rcSyncTaskTmp);
	rcSyncTaskFrame.DeflateRect(-10, -48, -10, -10);

	CRect rcLogTmp = rcLog;
	ScreenToClient(rcLogTmp);
	CRect rcLogFrame(rcLogTmp);
	rcLogFrame.DeflateRect(-10, -48, -10, -10);

	CRect rcExceptTmp = rcExcept;
	ScreenToClient(rcExceptTmp);
	CRect rcExceptFrame(rcExceptTmp);
	rcExceptFrame.DeflateRect(-10, -48, -10, -10);
	i8desk::ui::DrawWorkFrame(memDC, rcServerFrame, area_);
	i8desk::ui::DrawWorkFrame(memDC, rcDownloadFrame, area_);
	i8desk::ui::DrawWorkFrame(memDC, rcUpdateFrame, area_);
	i8desk::ui::DrawWorkFrame(memDC, rcSyncTaskFrame, area_);
	i8desk::ui::DrawWorkFrame(memDC, rcLogFrame, area_);
	i8desk::ui::DrawWorkFrame(memDC, rcExceptFrame, area_);

	i8desk::ui::DrawFrame(this, memDC, rcServer, outLine_);
	i8desk::ui::DrawFrame(this, memDC, rcDownload, outLine_);
	i8desk::ui::DrawFrame(this, memDC, rcUpdate, outLine_);
	i8desk::ui::DrawFrame(this, memDC, rcSyncTask, outLine_);
	i8desk::ui::DrawFrame(this, memDC, rcLog, outLine_);
	i8desk::ui::DrawFrame(this, memDC, rcExcept, outLine_);
}

void CDlgMonitor::OnSize(UINT flag, int x, int y)
{
	UPDATE_EASYSIZE;
	if( !::IsWindow(wndListLog_.GetSafeHwnd()) )
		return;

	wndListServer_.Invalidate();
	wndListDownload_.Invalidate();
	wndListUpdate_.Invalidate();
	wndListSyncTask_.Invalidate();
	wndListLog_.Invalidate();
	wndListCliExcept_.Invalidate();

	wndBtnServer_.Invalidate();
	wndBtnDownload_.Invalidate();
	wndBtnUpdate_.Invalidate();
	wndBtnSyncTask_.Invalidate();
	wndBtnCliExcept_.Invalidate();
}


void CDlgMonitor::OnLvnGetdispinfoListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;

	assert(itemIndex < monitorStatus_.ServersInfo.size());
	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_SERVER_NAME:
			text = monitorStatus_.ServersInfo[itemIndex]->SvrName ;
			if(monitorStatus_.ServersInfo[itemIndex]->SvrType == i8desk::MainServer )
				text += _T("(主)");
			else
				text += _T("(从)");
			break;
		case MASK_SERVER_CLICONNECT:
			stdex::ToString(monitorStatus_.ServersInfo[itemIndex]->ClientConnects, text);
			break;
		case MASK_SERVER_STATUS:
			svrStatus->SetStatus(monitorStatus_.ServersInfo[itemIndex]->Status == 1 ? true : false);
			text.clear();
			break;
		case MASK_SERVER_CONNECT:
			stdex::ToString(monitorStatus_.ServersInfo[itemIndex]->Connects, text);
			break;
		case MASK_SERVER_SPEED:
			i8desk::FormatSize(monitorStatus_.ServersInfo[itemIndex]->TotalSpeed, text);
			break;
		case MASK_SERVER_BYTES:
			i8desk::FormatSize(monitorStatus_.ServersInfo[itemIndex]->TotalReadData, text);
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgMonitor::OnLvnGetdispinfoListDownload(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;

	if( itemIndex != 0 )
		return;

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_DOWNLOAD_STATUS:
			if( itemIndex == 0 )
				text = monitorStatus_.DownloadResInfo.Status != 0 ? _T("正在下载") : _T("无下载任务");
			else
				text.clear();
			break;
		case MASK_DOWNLOAD_DOWNLOADING:
			stdex::ToString(monitorStatus_.DownloadResInfo.DownlaodingCount, text);
			text += _T("个资源");
			break;
		case MASK_DOWNLOAD_SPEED:
			i8desk::FormatSize(monitorStatus_.DownloadResInfo.TotalSpeed, text);
			text += _T("/S");
			break;
		case MASK_DOWNLOAD_WAITTING:
			stdex::ToString(monitorStatus_.DownloadResInfo.QueueCount, text);
			text += _T("个资源");
			break;
		case MASK_DOWNLOAD_CONNECT:
			stdex::ToString(monitorStatus_.DownloadResInfo.TotalConnect, text);
			text += _T("个连接数");
			break;
		case MASK_DOWNLOAD_TEMP_SIZE:
			i8desk::FormatSize(monitorStatus_.DownloadResInfo.TmpSize, text);
			break;
		case MASK_DOWNLOAD_LIMIT:
			if( monitorStatus_.DownloadResInfo.LimitSpeed == 0 )
				text = _T("无限制");
			else
			{
				stdex::ToString(monitorStatus_.DownloadResInfo.LimitSpeed , text);
				text += _T("KB/S");
			}
			break;
		case MASK_DOWNLOAD_CPUUSAGE:
			stdex::ToString(monitorStatus_.DownloadResInfo.CPUUsage, text);
			text += _T("%");
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgMonitor::OnLvnGetdispinfoListUpdate(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;


	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_UPDATEGAME_CLIENTS:
			stdex::ToString(monitorStatus_.UpdateInfo.TotalClients, text);
			break;
		case MASK_UPDATEGAME_SPEED:
			i8desk::FormatSize(monitorStatus_.UpdateInfo.TotalSpeed, text);
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgMonitor::OnLvnGetdispinfoListSyncTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;

	static stdex::tString text;
	unsigned long long speed = 0;

	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_SYNCTASK_CLIENTS:
			stdex::ToString(monitorStatus_.SyncTaskInfo.TotalClients, text);
			break;
		case MASK_SYNCTASK_SPEED:
			speed = monitorStatus_.SyncTaskInfo.TotalSpeed ;
			i8desk::FormatSize(speed, text);
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgMonitor::OnLvnGetdispinfoListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;

	if( i8desk::business::monitor::GetLogCount() < itemIndex )
		return;

	i8desk::LogRecord log;
	i8desk::business::monitor::GetLogRecord(itemIndex, log);

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_MODULE:
			text = log.plugin;
			break;
		case MASK_TIME:
			text = i8desk::FormatTime(log.time);
			break;
		case MASK_DESC:
			logMsg->SetTextColor(itemIndex, MASK_DESC, log.clr_);
			text = log.data;
			break;
		default:
			text.clear();
			break;
		}
		if( text.length() > (size_t)pItem->cchTextMax )
			text = text.substr(0, pItem->cchTextMax - 1);
		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}


void CDlgMonitor::OnLvnGetdispinfoListExcept(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	size_t itemIndex = pItem->iItem;
	if( i8desk::business::monitor::GetExceptCount() < itemIndex )
		return;

	std::tr1::tuple<stdex::tString, stdex::tString, stdex::tString, stdex::tString> info = i8desk::business::monitor::GetExceptInfo(itemIndex);
	
	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_EXCEPT_NAME:
			text = std::tr1::get<0>(info);
			break;
		case MASK_EXCEPT_IP:
			text = std::tr1::get<1>(info);
			break;
		case MASK_EXCEPT_TIME:
			text = std::tr1::get<2>(info);
			break;
		case MASK_EXCEPT_CONTENT:
			text = std::tr1::get<3>(info);
			content->SetTextColor(itemIndex, MASK_EXCEPT_CONTENT, RGB(255, 0, 0)); 
			break;
		default:
			text.clear();
			break;
		}
		if( text.length() > (size_t)pItem->cchTextMax )
			text = text.substr(0, pItem->cchTextMax - 1);
		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}


void CDlgMonitor::ONClickDownload(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMLV->iItem != 1 )
		return;

	if( pNMLV->iSubItem == 2 || pNMLV->iSubItem == 4 )
		OnBnClickedButtonMonitorDownload();
	else if( pNMLV->iSubItem == 6 )
	{
		i8desk::GetControlMgr().DeleteTmpFiles();
		return;
	}
	else if( pNMLV->iSubItem == 7 )
	{
		GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 7, 3);
		return;
	}
}


void CDlgMonitor::OnBnClickedButtonMonitorServer()
{
	GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 1, 1);
}

void CDlgMonitor::OnBnClickedButtonMonitorDownload()
{
	GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 3, 3);
}

void CDlgMonitor::OnBnClickedButtonMonitorUpdate()
{
	GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 3, 4);
}

void CDlgMonitor::OnBnClickedButtonMonitorSynctask()
{
	GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 4, 1);
}

void CDlgMonitor::OnBnClickedButtonMonitorExcept()
{
	GetParent()->PostMessage(i8desk::ui::WM_SELECT_TAB, 2, 1);
}
