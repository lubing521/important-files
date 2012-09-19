// WZDlgCmpBootTask.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgCmpBootTask.h"
#include "DlgStartupInfo.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../Business/BootTaskBusiness.h"
#include "../MessageBox.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"


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
		MASK_BOOT_NAME = 1, MASK_BOOT_AREA, MASK_BOOT_DATE, MASK_BOOT_FLAG, MASK_BOOT_TYPE
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_BOOT_NAME,	_T("名称"),			LVCFMT_LEFT, 70, MASK_BOOT_NAME, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_BOOT_AREA,	_T("区域"),			LVCFMT_LEFT, 70, MASK_BOOT_AREA, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_BOOT_DATE,	_T("过期日期"),	LVCFMT_LEFT, 120, MASK_BOOT_DATE, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_BOOT_FLAG,	_T("任务状态"),	LVCFMT_LEFT, 70, MASK_BOOT_FLAG, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_BOOT_TYPE,	_T("类型"),			LVCFMT_LEFT, 140, MASK_BOOT_TYPE, detail::GetTrait() );
	}
}

// WZDlgCmpBootTask 对话框

IMPLEMENT_DYNAMIC(WZDlgCmpBootTask, CDialog)

WZDlgCmpBootTask::WZDlgCmpBootTask(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgCmpBootTask::IDD, pParent)
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgCmpBootTask::~WZDlgCmpBootTask()
{
	workOutLine_.Detach();
}

void WZDlgCmpBootTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WZ_CMPBOOTTASK, wndListBootTasks_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CMPBOOTTASK_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CMPBOOTTASK_DEL, wndbtnDel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CMPBOOTTASK_MODIFY, wndbtnModify_);

}


BEGIN_MESSAGE_MAP(WZDlgCmpBootTask, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CMPBOOTTASK_ADD, &WZDlgCmpBootTask::OnBnClickedButtonBootTaskAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CMPBOOTTASK_DEL, &WZDlgCmpBootTask::OnBnClickedButtonBootTaskDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CMPBOOTTASK_MODIFY, &WZDlgCmpBootTask::OnBnClickedButtonBootTaskModify)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_CMPBOOTTASK, &WZDlgCmpBootTask::OnLvnGetdispinfoListBootTaskInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WZ_CMPBOOTTASK, &WZDlgCmpBootTask::OnNMRDblclkListBootTaskInfo)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgCmpBootTask 消息处理程序
BOOL WZDlgCmpBootTask::OnInitDialog()
{
	CDialog::OnInitDialog();

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTasks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTaskAreas());

	InitListCtrl(wndListBootTasks_);

	_ShowView();

	return TRUE;
}

void WZDlgCmpBootTask::OnBnClickedButtonBootTaskAdd()
{
	CDlgStartupInfo dlg(true, 0);
	if( dlg.DoModal() == IDOK )
		_ShowView();
}

void WZDlgCmpBootTask::OnBnClickedButtonBootTaskDel()
{
	if( wndListBootTasks_.GetSelectedCount() == 0 )
		return;
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的开机任务吗?"));

	if( msgDlg.DoModal() == IDCANCEL)
		return;

	int nSel = -1;


	typedef std::vector<DWORD> SelBootTask;
	SelBootTask vecIdx;
	while((nSel = wndListBootTasks_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		vecIdx.push_back(nSel);
	}

	for(SelBootTask::const_iterator rit = vecIdx.begin(); rit != vecIdx.end(); ++rit)
	{
		nSel = *rit;
		if( !i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetBootTasks(), curBootTasks_[nSel]->TID) )
			break;
	}

	_ShowView();

}

void WZDlgCmpBootTask::OnBnClickedButtonBootTaskModify()
{
	int nSel = wndListBootTasks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;

	assert(nSel < (int)curBootTasks_.size() );

	CDlgStartupInfo dlg(false, curBootTasks_[nSel].get());
	if( dlg.DoModal() == IDOK )
	{
		_ShowView();
	}
}

void WZDlgCmpBootTask::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::BootTaskTraits;

	curBootTasks_.clear();

	const BootTaskTraits::MapType &tasks = i8desk::GetDataMgr().GetBootTasks();

	stdex::map2vector(tasks, curBootTasks_);

	size_t count = curBootTasks_.size();
	wndListBootTasks_.SetItemCount(count);
}

void WZDlgCmpBootTask::OnNMRDblclkListBootTaskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	OnBnClickedButtonBootTaskModify();
}

void WZDlgCmpBootTask::OnLvnGetdispinfoListBootTaskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curBootTasks_.size() )
		return;

	const i8desk::data_helper::BootTaskTraits::ValueType &val = curBootTasks_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_BOOT_NAME:
			text = val->Name;
			break;
		case MASK_BOOT_AREA:
			text = i8desk::business::BootTask::GetAreaDesc(val);
			break;
		case MASK_BOOT_DATE:
			text = i8desk::FormatTime(val->EndDate);
			break;
		case MASK_BOOT_FLAG:
			text = val->Flag == 0 ? _T("固定执行") : _T("仅在还原保护时执行");
			break;
		case MASK_BOOT_TYPE:
			text = i8desk::business::BootTask::GetTypeDesc(val);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void WZDlgCmpBootTask::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgCmpBootTask::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndListBootTasks_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &workOutLine_);
	return TRUE;

}