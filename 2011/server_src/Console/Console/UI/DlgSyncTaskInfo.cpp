// DlgSyncTaskInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgSyncTaskInfo.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/utility.h"
#include "../Misc.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"

#include "../Business/SyncTaskBusiness.h"
#include "../Business/ServerBusiness.h"
#include "../AsyncDataHelper.h"
#include "../../../../include/UpdateGame.h"

#include <WindowsX.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgSyncTaskInfo 对话框
namespace
{
	void SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &IsDelFile)
	{
		i8desk::AsyncDataHelper<bool>(std::tr1::bind(&i8desk::manager::ControlMgr::SyncTaskOperate, 
			std::tr1::ref(i8desk::GetControlMgr()), cmd, gid, SID, TaskName, IsDelFile));
	}
}


using namespace i8desk::data_helper;
using namespace std::tr1::placeholders;

IMPLEMENT_DYNAMIC(CDlgSyncTaskInfo, CNonFrameChildDlg)

CDlgSyncTaskInfo::CDlgSyncTaskInfo(bool isAdd, const SyncTaskTraits::ElementType *parentsyncTask,
								   i8desk::data_helper::GameTraits::VectorType &games,
								    CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgSyncTaskInfo::IDD, pParent)
	, games_(games)
	, isAdd_(isAdd)
	, cursyncTask_(new SyncTaskTraits::ElementType)
	, parentsyncTask_(parentsyncTask)
	, syncTaskName_(_T(""))
	, maxSpeed_(_T("0"))

{
	if( isAdd_ )
		utility::Strcpy(cursyncTask_->SID, utility::CreateGuidString());
	else
	{
		*cursyncTask_ = *parentsyncTask_;
	}	

	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));

}

CDlgSyncTaskInfo::~CDlgSyncTaskInfo()
{
	outboxLine_.Detach();
}

void CDlgSyncTaskInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SYNCTASK_NAME, syncTaskName_);
	DDX_Text(pDX, IDC_EDIT_SYNCTASK_SPEED, maxSpeed_);

	DDX_Control(pDX, IDC_EDIT_SYNCTASK_NAME, wndEditsyncTaskName_);
	DDX_Control(pDX, IDC_EDIT_SYNCTASK_SPEED, wndEdimaxSpeed_);

	DDX_Control(pDX, IDOK, wndbtnOk_);
	DDX_Control(pDX, IDCANCEL, wndbtnCancel_);


	DDX_Control(pDX, IDC_STATIC_SYNCTASK_NAME, wndTipSyncTaskName_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_FROM, wndTipFrom_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_TO, wndTipTo_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_SVR_NAME, wndTipSvrName_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_SVR_IP, wndTipSvrIP_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_SRC_DIR, wndTipSvrDir_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_DST_NAME, wndTipCliName_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_DST_IP, wndTipCliIP_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_DST_DIR, wndTipCliDir_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_SPEED, wndTipSpeed_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_SPEED_PARAM, wndTipSpeedKB_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_SET, wndGroupSyncTaskSet_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_INFO, wndGroupSyncTaskInfo_);
	DDX_Control(pDX, IDC_STATIC_SYNC_TASK_INFO_SRCDIR_PARAM, wndTipSvrDirParam_ );



	DDX_Control(pDX, IDC_COMBO_SYNC_TASK_INFO_SVR_NAME, wndComboSvrName_);
	DDX_Control(pDX, IDC_COMBO_SYNC_TASK_INFO_SVR_IP, wndComboSvrIP_);

	DDX_Control(pDX, IDC_COMBO_SYNC_TASK_INFO_DST_NAME, wndComboCliName_);
	DDX_Control(pDX, IDC_COMBO_SYNC_TASK_INFO_DST_IP, wndComboCliIP_);
	DDX_Control(pDX, IDC_COMBO_SYNC_TASK_INFO_DST_DIR, wndComboCliDir_);
}


BEGIN_MESSAGE_MAP(CDlgSyncTaskInfo, CNonFrameChildDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CDlgSyncTaskInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSyncTaskInfo::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_SYNC_TASK_INFO_SVR_NAME, &CDlgSyncTaskInfo::OnCbnSelchangeComboSynctaskSvrName)
	ON_CBN_SELCHANGE(IDC_COMBO_SYNC_TASK_INFO_DST_NAME, &CDlgSyncTaskInfo::OnCbnSelchangeComboSynctaskCliName)
END_MESSAGE_MAP()



namespace
{
	struct AddCombo
	{
		CComboBox &combo_;
		AddCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(LPCTSTR name, LPCTSTR sid)
		{
			int index = combo_.AddString(name);
			combo_.SetItemData(index, reinterpret_cast<DWORD_PTR>(sid));
		}
	};

	template< typename TreeCtrlT, typename PathsT >
	void InitGameTree(TreeCtrlT &tree, const PathsT &paths)
	{
		tree.ModifyStyle(TVS_CHECKBOXES, 0);
		tree.ModifyStyle(0, TVS_CHECKBOXES);

		std::map<stdex::tString, HTREEITEM> classItems;
		for(typename PathsT::const_iterator iter = paths.begin();
			iter != paths.end(); ++iter)
		{
			HTREEITEM hclassItem = tree.InsertItem((*iter).c_str());
			classItems[(*iter)] = hclassItem;
		}
	}

}


bool CDlgSyncTaskInfo::_CheckData()
{
	UpdateData(TRUE);

	if( utility::IsEmpty((LPCTSTR)syncTaskName_) ) 
	{
		CMessageBox msgDlg(_T("提示"), _T("没有设定任务名称"));
		msgDlg.DoModal();
		wndEditsyncTaskName_.SetFocus();
		return false;
	}

	// TaskName
	utility::Strcpy(cursyncTask_->Name, (LPCTSTR)syncTaskName_);
	if( !i8desk::business::SyncTask::HasSameTaskName(cursyncTask_) )
	{
		CMessageBox msgDlg(_T("提示"),_T("同步任务中已有相同的任务名称，请勿重复添加。\r\n请重新设置同步任务名称"));
		msgDlg.DoModal();
		wndEditsyncTaskName_.SetFocus();
		return false;
	}

	// Speed
	cursyncTask_->Speed = _ttoi((LPCTSTR)maxSpeed_);

	// Server ID
	int cliSel = wndComboCliName_.GetCurSel();
	typedef i8desk::data_helper::ServerTraits::ElementType ServerType;
	ServerType *cliSvr = reinterpret_cast<ServerType *>(wndComboCliName_.GetItemData(cliSel));
	utility::Strcpy(cursyncTask_->SvrID, cliSvr->SvrID);

	// From IP
	CString ip;
	wndComboSvrIP_.GetWindowText(ip);
	if( utility::IsEmpty((LPCTSTR)ip) || !i8desk::IsValidIP((LPCTSTR)ip) )
	{
		CMessageBox msgDlg(_T("提示"), _T("源IP设置错误"));
		msgDlg.DoModal();
		wndComboSvrIP_.SetFocus();
		return false;
	}
	cursyncTask_->SoucIp = ::inet_addr((LPCSTR)CT2A(ip));

	// To IP
	wndComboCliIP_.GetWindowText(ip);
	if( utility::IsEmpty((LPCTSTR)ip) || !i8desk::IsValidIP((LPCTSTR)ip) )
	{
		CMessageBox msgDlg(_T("提示"), _T("目的IP设置错误"));
		msgDlg.DoModal();
		wndComboCliIP_.SetFocus();
		return false;
	}
	cursyncTask_->DestIp = ::inet_addr((LPCSTR)CT2A(ip));

	// To Dirs
	CString nodeDir;
	wndComboCliDir_.GetWindowText(nodeDir);
	utility::Strcpy(cursyncTask_->NodeDir, (LPCTSTR)nodeDir) ;

	// 避免不同任务同步到本机相同路径
	if( i8desk::business::SyncTask::HasSameNodeDir(cursyncTask_) )
	{
		CMessageBox msgDlg(_T("提示"), _T("不允许有同步到本机相同位置的设置\n请修改目标服务器或设置一个不同的同步盘符\n"));
		msgDlg.DoModal();
		return false;
	}

	// 避免同步源路径与目标路径相同
	if(i8desk::business::SyncTask::HasSamePath(cursyncTask_, games_))
	{
		CMessageBox msgDlg(_T("提示"), _T("避免同步源路径与目标路径相同\n请修改目标服务器或设置一个不同的同步盘符\n"));
		msgDlg.DoModal();
		return false;
	}


	return true;
}


// CDlgSyncTaskInfo 消息处理程序

BOOL CDlgSyncTaskInfo::OnInitDialog()
{
	// 设置ComboBox皮肤
	CNonFrameChildDlg::OnInitDialog();
	
	syncTaskName_ = cursyncTask_->Name;

	// Server Name
	i8desk::business::Server::GetAllService(AddCombo(wndComboSvrName_));
	i8desk::business::Server::GetAllService(AddCombo(wndComboCliName_));

	stdex::tString svrSID;
	for(int i = 0; i != wndComboSvrName_.GetCount(); ++i)
	{
		svrSID = reinterpret_cast<LPCTSTR>(wndComboSvrName_.GetItemData(i));
		if( i8desk::business::Server::IsMainServer(i8desk::business::Server::GetServiceBySID(svrSID)) )
		{
			wndComboSvrName_.SetCurSel(i);
			break;
		}
	}
	wndComboSvrName_.EnableWindow(FALSE);

	
	// Dest Dir
	for(TCHAR drv = 'D'; drv <= 'Z'; drv++) 
	{
		TCHAR szDrv[] = {drv, 0};
		wndComboCliDir_.AddString(szDrv);
	}


	if( isAdd_ ) 
	{
		SetTitle(_T("添加同步任务"));
		syncTaskName_ = _T("默认");
		wndbtnOk_.SetWindowText(_T("添加"));


		wndComboCliName_.SetCurSel(0);
		wndComboCliDir_.SetCurSel(0);
		wndComboSvrIP_.SetCurSel(0);
		wndComboCliIP_.SetCurSel(0);
	}
	else 
	{
		CString caption = _T("修改同步任务: ");
		caption += cursyncTask_->Name;
		SetTitle(caption);
		wndbtnOk_.SetWindowText(_T("修改"));
		stdex::tString str;
		stdex::ToString(cursyncTask_->Speed, str);
		maxSpeed_ = str.c_str();
		wndComboCliName_.SelectString(-1, i8desk::business::Server::GetServiceBySID(svrSID)->SvrName);
		
		CString nodeDir;
		nodeDir.Append(cursyncTask_->NodeDir, 1);
		wndComboCliDir_.SelectString(-1, nodeDir);


		for(int i = 0; i != wndComboCliName_.GetCount(); ++i)
		{
			stdex::tString cliSID = (LPCTSTR)wndComboCliName_.GetItemData(i);
			if( cliSID == cursyncTask_->SvrID )
			{
				wndComboCliName_.SetCurSel(i);
				break;
			}
		}
	}

	// 选择当前服务器IP
	OnCbnSelchangeComboSynctaskSvrName();
	OnCbnSelchangeComboSynctaskCliName();
	
	

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgSyncTaskInfo::OnBnClickedOk()
{
	if( !_CheckData() )
		return;

	if( isAdd_ )
	{
		if( !i8desk::business::SyncTask::AddSyncTask(cursyncTask_, games_) )
		{
			CMessageBox msgDlg(_T("提示"), _T("添加同步任务失败\n"));
			msgDlg.DoModal();
			return;
		}

		stdex::tString taskname = cursyncTask_->Name;
		stdex::tString SID = cursyncTask_->SID;
		SyncTaskOperate(AddGame, 0, SID, taskname, false);

		CMessageBox msgdlg(_T("提示"), _T("是否立即开始同步任务"));
		if(msgdlg.DoModal() == IDOK)
			SyncTaskOperate(StartTask, 0, SID, taskname, false);
	}
	else
	{
		if( !i8desk::business::SyncTask::ModifySyncTask(cursyncTask_) )
		{
			CMessageBox msgDlg(_T("提示"), _T("修改同步任务失败\n"));
			msgDlg.DoModal();
			return;
		}
	}	



	OnOK();
}

void CDlgSyncTaskInfo::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgSyncTaskInfo::OnCbnSelchangeComboSynctaskSvrName()
{
	int index = wndComboSvrName_.GetCurSel();
	if( index == -1 )
		return;

	stdex::tString sid = (LPCTSTR)wndComboSvrName_.GetItemData(index);
	const i8desk::data_helper::ServerTraits::ValueType &val = i8desk::business::Server::GetServiceBySID(sid);

	wndComboSvrIP_.ResetContent();
	if( val->Ip1 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip1).c_str());
	if( val->Ip2 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip2).c_str());
	if( val->Ip3 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip3).c_str());
	if( val->Ip4 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip4).c_str());

	if( isAdd_ )
		wndComboSvrIP_.SetCurSel(0);
	else
		wndComboSvrIP_.SelectString(-1, i8desk::IP2String(cursyncTask_->SoucIp).c_str());

}


void CDlgSyncTaskInfo::OnCbnSelchangeComboSynctaskCliName()
{
	int index = wndComboCliName_.GetCurSel();
	if( index == -1 )
		return;

	stdex::tString sid = (LPCTSTR)wndComboCliName_.GetItemData(index);
	const i8desk::data_helper::ServerTraits::ValueType &val = i8desk::business::Server::GetServiceBySID(sid);

	wndComboCliIP_.ResetContent();
	if( val->Ip1 != 0 )
		wndComboCliIP_.AddString(i8desk::IP2String(val->Ip1).c_str());
	if( val->Ip2 != 0 )
		wndComboCliIP_.AddString(i8desk::IP2String(val->Ip2).c_str());
	if( val->Ip3 != 0 )
		wndComboCliIP_.AddString(i8desk::IP2String(val->Ip3).c_str());
	if( val->Ip4 != 0 )
		wndComboCliIP_.AddString(i8desk::IP2String(val->Ip4).c_str());

	if( isAdd_ )
		wndComboCliIP_.SetCurSel(0);
	else
		wndComboCliIP_.SelectString(-1, i8desk::IP2String(cursyncTask_->DestIp).c_str());
}



BOOL CDlgSyncTaskInfo::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	//CRect rcMultiText;
	////WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcMultiText);
	//wndTreeSvrDir_.GetWindowRect(rcMultiText);
	//i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}