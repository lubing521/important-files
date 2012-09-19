// WZDlgSyncTask.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgSyncTask.h"
#include "DlgSyncTaskInfoEx.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/I8Type.hpp"
#include "../MessageBox.h"
#include "../Business/SyncTaskBusiness.h"
#include "../Business/ServerBusiness.h"

#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"

// WZDlgSyncTask 对话框

IMPLEMENT_DYNAMIC(WZDlgSyncTask, CDialog)

WZDlgSyncTask::WZDlgSyncTask(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgSyncTask::IDD, pParent)
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgSyncTask::~WZDlgSyncTask()
{
	workOutLine_.Detach();
}

void WZDlgSyncTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_WZ_SYNCTASK_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_SYNCTASK_DEL, wndbtnDel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_SYNCTASK_MODIFY, wndbtnModify_);

	DDX_Control(pDX, IDC_LIST_WZ_SYNCTASK, wndListSyncTasks_);

}


BEGIN_MESSAGE_MAP(WZDlgSyncTask, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SYNCTASK_ADD, &WZDlgSyncTask::OnBnClickedButtonSyncTaskAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SYNCTASK_DEL, &WZDlgSyncTask::OnBnClickedButtonSyncTaskDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SYNCTASK_MODIFY, &WZDlgSyncTask::OnBnClickedButtonSyncTaskModify)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_SYNCTASK, &WZDlgSyncTask::OnLvnGetdispinfoListSyncTaskInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WZ_SYNCTASK, &WZDlgSyncTask::OnNMRDblclkListSyncTaskInfo)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

namespace
{
	namespace detail
	{
		CCustomColumnTrait *GetTrait()
		{
			CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
			rowTrait->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return rowTrait;
		}

	}
	enum
	{ 
		MASK_SYNCTASK_NAME = 1, MASK_SYNCTASK_SRC_NAME, MASK_SYNCTASK_DST_NAME,  MASK_SYNCTASK_DST_PATH, MASK_SYNCTASK_MAXSPEED
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_SYNCTASK_NAME,		_T("名称"),		LVCFMT_LEFT, 65, MASK_SYNCTASK_NAME,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SYNCTASK_SRC_NAME,	_T("源服务器名"),		LVCFMT_LEFT, 150, MASK_SYNCTASK_SRC_NAME,	detail::GetTrait() );
		//listCtrl.InsertColumnTrait(MASK_SYNCTASK_SRC_IP,	_T("源IP"),		LVCFMT_LEFT, 80, MASK_SYNCTASK_SRC_IP,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SYNCTASK_DST_NAME,	_T("目标务器名"),		LVCFMT_LEFT, 150, MASK_SYNCTASK_DST_NAME,	detail::GetTrait() );
		//listCtrl.InsertColumnTrait(MASK_SYNCTASK_DST_IP,	_T("目标IP"),		LVCFMT_LEFT, 80, MASK_SYNCTASK_DST_IP,	detail::GetTrait() );
		//listCtrl.InsertColumnTrait(MASK_SYNCTASK_SRC_PATH,	_T("源目录"),		LVCFMT_LEFT, 80, MASK_SYNCTASK_SRC_PATH,detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SYNCTASK_DST_PATH,	_T("目标目录"),	LVCFMT_LEFT, 80, MASK_SYNCTASK_DST_PATH,detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SYNCTASK_MAXSPEED,	_T("限速"),		LVCFMT_LEFT, 65, MASK_SYNCTASK_MAXSPEED,detail::GetTrait() );
	}
}
// WZDlgSyncTask 消息处理程序
BOOL WZDlgSyncTask::OnInitDialog()
{
	CDialog::OnInitDialog();

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetSyncTasks());

	InitListCtrl(wndListSyncTasks_);

	_ShowView();

	return TRUE;
}

void WZDlgSyncTask::OnBnClickedButtonSyncTaskAdd()
{
	CDlgSyncTaskInfoEx dlg(isadd, 0);
	if( dlg.DoModal() == IDOK )
		_ShowView();
}

void WZDlgSyncTask::OnBnClickedButtonSyncTaskDel()
{
	if( wndListSyncTasks_.GetSelectedCount() == 0 )
		return;
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的同步任务吗?"));

	if( msgDlg.DoModal() == IDCANCEL)
		return;

	int nSel = -1;


	typedef std::vector<DWORD> SelPushGame;
	SelPushGame vecIdx;
	while((nSel = wndListSyncTasks_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		vecIdx.push_back(nSel);
	}

	for(SelPushGame::const_iterator rit = vecIdx.begin(); rit != vecIdx.end(); ++rit)
	{
		stdex::tString SID = curSyncTasks_[*rit]->SID;
		if(!i8desk::business::SyncTask::DelSyncTask(SID))
			break;
		//if( !i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetSyncTasks(), curSyncTasks_[nSel]->SID) )
		//	break;
	}

	_ShowView();
}

void WZDlgSyncTask::OnBnClickedButtonSyncTaskModify()
{
	int nSel = wndListSyncTasks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;

	assert(nSel < (int)curSyncTasks_.size() );

	CDlgSyncTaskInfoEx dlg(ismodify, curSyncTasks_[nSel].get());
 	if(dlg.DoModal() == IDOK )
 		_ShowView();
}

void WZDlgSyncTask::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::SyncTaskTraits;

	curSyncTasks_.clear();

	const SyncTaskTraits::MapType &tasks = i8desk::GetDataMgr().GetSyncTasks();

	stdex::map2vector(tasks, curSyncTasks_);

	size_t count = curSyncTasks_.size();
	wndListSyncTasks_.SetItemCount(count);
}

void WZDlgSyncTask::OnNMRDblclkListSyncTaskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	OnBnClickedButtonSyncTaskModify();
}

void WZDlgSyncTask::OnLvnGetdispinfoListSyncTaskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curSyncTasks_.size() )
		return;

	const i8desk::data_helper::SyncTaskTraits::ValueType &val = curSyncTasks_[itemIndex];

	static stdex::tString text;
	stdex::tString SvrID;
	stdex::tOstringstream os;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_SYNCTASK_NAME:
			text = val->Name;
			break;
		case MASK_SYNCTASK_SRC_NAME:
			{
				const i8desk::data_helper::ServerTraits::ValueType &server = i8desk::business::Server::GetMainServer();
				os << i8desk::business::Server::GetNameBySID(server->SvrID) << _T("(") 
					<< i8desk::IP2String(val->SoucIp) << _T(")") ;
				text = os.str();
			}
			break;
		case MASK_SYNCTASK_DST_NAME:
			{
				SvrID =  val->SvrID;
				os << i8desk::business::Server::GetNameBySID(SvrID) << _T("(") 
					<< i8desk::IP2String(val->DestIp) << _T(")") ;
				text = os.str();
			}
			break;
		case MASK_SYNCTASK_DST_PATH:
			text = val->NodeDir;
			break;
		case MASK_SYNCTASK_MAXSPEED:
			if( val->Speed == 0 )
				text = _T("不限速");
			else
			{
				stdex::ToString(val->Speed, text, 0);
				text += _T("KB/S");
			}
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void WZDlgSyncTask::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgSyncTask::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndListSyncTasks_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &workOutLine_);

	return TRUE;

}