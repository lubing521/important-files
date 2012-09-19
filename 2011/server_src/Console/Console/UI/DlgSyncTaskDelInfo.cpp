#include "stdafx.h"
#include "../Console.h"
#include "DlgSyncTaskDelInfo.h"

#include "../Business/SyncTaskBusiness.h"
#include "../ManagerInstance.h"
#include "../AsyncDataHelper.h"
#include "../MessageBox.h"

#include "../ui/Skin/SkinMgr.h"

#include "../../../../include/UpdateGame.h"
#include "../../../../include/Extend STL/UnicodeStl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	void SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &IsDelFile)
	{
		i8desk::AsyncDataHelper<bool>(std::tr1::bind(&i8desk::manager::ControlMgr::SyncTaskOperate, 
			std::tr1::ref(i8desk::GetControlMgr()), cmd, gid, SID, TaskName, IsDelFile));
	}
}
// CDlgSyncTaskDelInfo 对话框

IMPLEMENT_DYNAMIC(CDlgSyncTaskDelInfo, CNonFrameChildDlg)

CDlgSyncTaskDelInfo::CDlgSyncTaskDelInfo(const std::vector<long> &games, const std::vector<stdex::tString> &Sids,
										 bool viewstatus, CWnd* pParent /*=NULL*/)
: CNonFrameChildDlg(CDlgSyncTaskDelInfo::IDD, pParent)
, curgames_(games)
, curSids_(Sids)
, ViewStatus_(viewstatus)
{

}

CDlgSyncTaskDelInfo::~CDlgSyncTaskDelInfo()
{
}

void CDlgSyncTaskDelInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SYNCTASK_DELINFO_DELALL, wndRadioDelSyncAll_);
	DDX_Control(pDX, IDC_RADIO_SYNCTASK_DELINFO_DELRECORD, wndRadioDelSyncRecord_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_DELINFO_OK, wndBtnOk_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_DELINFO_CANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_DELINFO_PARAMALL, wndLabelAll_);
	DDX_Control(pDX, IDC_STATIC_SYNCTASK_DELINFO_PARAMONE, wndLabelOne_);


}


BEGIN_MESSAGE_MAP(CDlgSyncTaskDelInfo, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_DELINFO_OK, &CDlgSyncTaskDelInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_DELINFO_CANCEL, &CDlgSyncTaskDelInfo::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSyncTaskDelInfo 消息处理程序

BOOL CDlgSyncTaskDelInfo::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("删除同步游戏"));

	if(ViewStatus_ == 0)
	{
		wndLabelAll_.ShowWindow(SW_SHOW);
		wndLabelOne_.ShowWindow(SW_HIDE);
	}
	else
	{
		wndLabelAll_.ShowWindow(SW_HIDE);
		stdex::tOstringstream os;
		os << _T("针对[ ") << i8desk::business::SyncTask::GetTaskNameBySID(curSids_[0])  << _T(" ]同步任务, 做以下操作, 游戏为你所选中选中的游戏");
		wndLabelOne_.SetWindowText(os.str().c_str());
		wndLabelOne_.ShowWindow(SW_SHOW);
	}

	 wndRadioDelSyncRecord_.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
}



void CDlgSyncTaskDelInfo::OnBnClickedOk()
{
	bool delRecord	= wndRadioDelSyncRecord_.GetCheck() == BST_CHECKED;
	bool delAll		= wndRadioDelSyncAll_.GetCheck() == BST_CHECKED;

	if(!(delRecord || delAll))
	{
		CMessageBox msgDlg(_T("提示"), _T("	请选择删除同步游戏的方式"));
		msgDlg.DoModal();
	}

	for(auto iter = curSids_.cbegin(); iter != curSids_.cend(); ++iter)
	{
		stdex::tString SID = *iter;
		stdex::tString taskname = i8desk::business::SyncTask::GetTaskNameBySID(SID);
		for(auto giter = curgames_.cbegin(); giter != curgames_.cend(); ++giter)
		{
			if(delAll)
			{
				SyncTaskOperate(DeleteGame, *giter, SID, taskname, true);
				i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetSyncGames(), std::make_pair(SID.c_str(), *giter)); 
				continue;
			}

			if(delRecord)
			{
				SyncTaskOperate(DeleteGame, *giter, SID, taskname, false);
				i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetSyncGames(), std::make_pair(SID.c_str(), *giter)); 
			}

		}
	}

	OnOK();
}

void CDlgSyncTaskDelInfo::OnBnClickedCancel()
{
	OnCancel();
}







