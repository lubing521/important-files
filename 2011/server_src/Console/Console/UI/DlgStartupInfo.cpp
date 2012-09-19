// DlgStartupInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartupInfo.h"

#include <set>

#include "../../../../include/Utility/utility.h"
#include "../Business/BootTaskBusiness.h"
#include "../Business/AreaBusiness.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../MessageBox.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupInfo 对话框

IMPLEMENT_DYNAMIC(CDlgStartupInfo, CNonFrameChildDlg)

CDlgStartupInfo::CDlgStartupInfo(bool isAdd, const BootTaskTraits::ElementType *val, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgStartupInfo::IDD, pParent)
	, taskName_(_T(""))
	, isAdd_(isAdd)
	, curBootTask_(new BootTaskTraits::ElementType)
{	
	if( isAdd_ )
	{
		utility::Strcpy(curBootTask_->TID, utility::CreateGuidString());
	}
	else
	{
		memcpy(curBootTask_.get(), val, sizeof(BootTaskTraits::ElementType));
	}
}

CDlgStartupInfo::~CDlgStartupInfo()
{
}

void CDlgStartupInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STARTUP_INFO_NAME, taskName_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_INFO_NAME, wndEdittaskName_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTUP_INFO_TIME, wndTime_);
	DDX_Control(pDX, IDC_RADIO_STARTUP_CLIENT_MENU, wndRadioRunAtMenu_);
	DDX_Control(pDX, IDC_RADIO_STARTUP_CLIENT_STARTUP, wndRadioRunAtBoot_);
	DDX_Control(pDX, IDC_RADIO_STARTUP_INFO_DIRECT_RUN, wndRadioDirectRun_);
	DDX_Control(pDX, IDC_RADIO_STARTUP_INFO_PROTO_RUN, wndRadioProtoRun_);

	DDX_Control(pDX, IDC_TREE_STARTUP_INFO_AREA, wndTreeAreas_);
	DDX_Control(pDX, IDC_COMBO_STARTUP_INFO_TYPE, wndComboType_);

	DDX_Control(pDX, IDC_STATIC_SATRTUP_INFO_SET, wndGroupSet_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_NAME, wndLableName_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_TIME, wndLableTime_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_TYPE, wndLableType_);

	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

	DDX_Control(pDX, IDC_TREE_STARTUP_INFO_RUN_DATE, wndTreeTimeRange_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTUP_START_TIME, wndTimeStart_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTUP_END_TIME, wndTimeEnd_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_DELAY_TIME, wndEditDelayTime_);
	DDX_Control(pDX, IDC_CHECK_STARTUP_STATUS, wndCheckboxStatus_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_AREA, wndTipArea_);
	DDX_Control(pDX, IDC_STATIC_SATRTUP_INFO_DATE, wndTipRunDate_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_START_TIME, wndTipStartTime_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_END_TIME, wndTipEndTime_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_RUN_TYPE, wndTipRunType_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_DELAY, wndTipDelay_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_INFO_DELAY_SECOND, wndTipDelaySecond_);
}


BEGIN_MESSAGE_MAP(CDlgStartupInfo, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgStartupInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgStartupInfo::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_STARTUP_INFO_PROTO_RUN, &CDlgStartupInfo::OnBnClickedProto)
	ON_BN_CLICKED(IDC_RADIO_STARTUP_INFO_DIRECT_RUN, &CDlgStartupInfo::OnBnClickedDirect)
	ON_CBN_SELCHANGE(IDC_COMBO_STARTUP_INFO_TYPE, &CDlgStartupInfo::OnCbnSelchangeComboStartupInfoType)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()




// CDlgStartupInfo 消息处理程序

BOOL CDlgStartupInfo::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("开机任务管理器"));

	// 切换页面
	dlgDesktop_.Create(CDlgStartupParamDesktop::IDD,	this);
	dlgIcon_.Create(CDlgStartupParamIocn::IDD,			this);
	dlgRunCli_.Create(CDlgStartupParamRunClient::IDD,	this);
	dlgRunSvr_.Create(CDlgStartupParamRunSvr::IDD,		this);
	dlgDel_.Create(CDlgStartupParamDel::IDD,			this);
	dlgUpdate_.Create(CDlgStartupParamDirectUpdate::IDD,this);

	wndMgr_.Register(CDlgStartupParamDesktop::IDD,			&dlgDesktop_);
	wndMgr_.Register(CDlgStartupParamIocn::IDD,				&dlgIcon_);
	wndMgr_.Register(CDlgStartupParamRunClient::IDD,		&dlgRunCli_);
	wndMgr_.Register(CDlgStartupParamRunSvr::IDD,			&dlgRunSvr_);
	wndMgr_.Register(CDlgStartupParamDel::IDD,				&dlgDel_);
	wndMgr_.Register(CDlgStartupParamDirectUpdate::IDD,		&dlgUpdate_);

	wndMgr_.SetDestWindow(GetDlgItem(IDC_STATIC_STARTUP_SPECIAL));

	params_[CDlgStartupParamDesktop::IDD]			= &dlgDesktop_;
	params_[CDlgStartupParamIocn::IDD]				= &dlgIcon_;
	params_[CDlgStartupParamRunSvr::IDD]			= &dlgRunSvr_;
	params_[CDlgStartupParamRunClient::IDD]			= &dlgRunCli_;
	params_[CDlgStartupParamDel::IDD]				= &dlgDel_;
	params_[CDlgStartupParamDirectUpdate::IDD]		= &dlgUpdate_;

	

	// Type
	i8desk::business::BootTask::TypeDesc(
		std::tr1::bind(&CComboBox::AddString, &wndComboType_, std::tr1::placeholders::_1),
		std::tr1::bind(&CComboBox::SetItemData, &wndComboType_, std::tr1::placeholders::_1, std::tr1::placeholders::_2));

	using i8desk::data_helper::AreaTraits;
	AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);

	using i8desk::data_helper::BootTaskAreaTraits;
	BootTaskAreaTraits::MapType &bootAreas = i8desk::GetDataMgr().GetBootTaskAreas();
	i8desk::GetDataMgr().GetAllData(bootAreas);


	// Area
	wndTreeAreas_.ModifyStyle(TVS_CHECKBOXES, 0);
	wndTreeAreas_.ModifyStyle(0, TVS_CHECKBOXES);
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		HTREEITEM hItem = wndTreeAreas_.InsertItem(iter->second->Name);
		wndTreeAreas_.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(iter->second.get()));

		if( i8desk::business::Area::IsDefaultArea(iter->second) )
			wndTreeAreas_.SetCheck(hItem);
	}

	// Time Date
	wndTreeTimeRange_.ModifyStyle(TVS_CHECKBOXES, 0);
	wndTreeTimeRange_.ModifyStyle(0, TVS_CHECKBOXES);

	struct AddDate
	{
		CTreeCtrl &tree_;
		AddDate(CTreeCtrl &tree)
			: tree_(tree)
		{}

		void operator()(int type, LPCTSTR text)
		{
			HTREEITEM hTree = tree_.InsertItem(text);
			tree_.SetItemData(hTree, type);
			tree_.SetCheck(hTree);
		}
	};
	i8desk::business::BootTask::GetRunDate(AddDate(wndTreeTimeRange_));

	if( isAdd_ )
	{
		wndComboType_.SetCurSel(0);
		wndEditDelayTime_.SetWindowText(_T("60"));
		wndCheckboxStatus_.SetCheck(BST_CHECKED);
		wndRadioDirectRun_.SetCheck(BST_CHECKED);
		wndRadioRunAtBoot_.SetCheck(BST_CHECKED);

		CTime time = CTime::GetCurrentTime();

		time += CTimeSpan(30, 0, 0, 0);
		CTime endTime(time.GetYear(), time.GetMonth(), time.GetDay(), 0, 0, 0);
		wndTime_.SetTime(&endTime);


		time = CTime::GetCurrentTime();
		CTime start(time.GetYear(), time.GetMonth(), time.GetDay(), 0, 0, 0);
		CTime end(time.GetYear(), time.GetMonth(), time.GetDay(), 23, 59, 59);

		wndTimeStart_.SetTime(&start);
		wndTimeEnd_.SetTime(&end);
	}
	else
	{
		// Name
		taskName_ = curBootTask_->Name;
		// Protocol
		wndRadioDirectRun_.SetCheck(curBootTask_->Flag == 0 ? BST_CHECKED : BST_UNCHECKED);
		wndRadioProtoRun_.SetCheck(curBootTask_->Flag == 1 ? BST_CHECKED : BST_UNCHECKED);
	
		// 选中一周中日期
		HTREEITEM hRoot = wndTreeTimeRange_.GetRootItem(); 
		while( hRoot ) 
		{ 
			int dstMask = wndTreeTimeRange_.GetItemData(hRoot);

			if( dstMask & curBootTask_->RunDate )
				wndTreeTimeRange_.SetCheck(hRoot);

			hRoot = wndTreeTimeRange_.GetNextItem(hRoot, TVGN_NEXT);
		}
		
		// 启动方式
		if( i8desk::business::BootTask::IsRunAtBoot(curBootTask_) )
			wndRadioRunAtBoot_.SetCheck(BST_CHECKED);
		else
			wndRadioRunAtMenu_.SetCheck(BST_CHECKED);
			
		// 启用Status
		wndCheckboxStatus_.SetCheck(curBootTask_->Status ? BST_CHECKED : BST_UNCHECKED);

		// 时间
		CTime time(curBootTask_->EndDate);
		wndTime_.SetTime(&time);

		// 起始时间段
		CTime startTime(curBootTask_->StartTime);
		wndTimeStart_.SetTime(&startTime);

		// 结束时间段
		CTime endTime(curBootTask_->EndTime);
		wndTimeEnd_.SetTime(&endTime);

		// 运行后延迟时间
		wndEditDelayTime_.SetWindowText(stdex::ToString<stdex::tString>(curBootTask_->DelayTime).c_str());

		// 设置选择类型
		wndComboType_.SelectString(-1, i8desk::business::BootTask::GetTypeDesc(curBootTask_).c_str());

		for(BootTaskAreaTraits::MapConstIterator iter = bootAreas.begin();
			iter != bootAreas.end(); ++iter)
		{
			if( utility::Strcmp(curBootTask_->TID, iter->second->TID) == 0 )
				aids_.insert(iter->second->AID);
		}	

		// 设置选择区域
		for(HTREEITEM hItem = wndTreeAreas_.GetRootItem(); 
			hItem != NULL; hItem = wndTreeAreas_.GetNextSiblingItem(hItem)) 
		{
			typedef i8desk::data_helper::AreaTraits::ElementType AreaElement;
			AreaElement *val = reinterpret_cast<AreaElement *>(wndTreeAreas_.GetItemData(hItem));
			assert(val);

			AIDs::const_iterator iter = aids_.find(val->AID);
			if( iter != aids_.end() )
				wndTreeAreas_.SetCheck(hItem, TRUE);
		}
	}

	OnCbnSelchangeComboStartupInfoType();

	// 设置参数
	if( !isAdd_ )
	{
		i8desk::ParamInterface *param = params_[wndMgr_.GetCurrentID()];
		assert(param);
		param->SetParam(curBootTask_->Parameter);
	}

	UpdateData(FALSE);


	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgStartupInfo::OnBnClickedProto()
{
	wndRadioDirectRun_.SetCheck(FALSE);
	wndRadioProtoRun_.SetCheck(TRUE); 
}

void CDlgStartupInfo::OnBnClickedDirect()
{
	wndRadioDirectRun_.SetCheck(TRUE);
	wndRadioProtoRun_.SetCheck(FALSE); 
}

void CDlgStartupInfo::OnDestroy()
{
	CNonFrameChildDlg::OnDestroy();

	dlgDesktop_.DestroyWindow();
	dlgIcon_.DestroyWindow();
	dlgDel_.DestroyWindow();
	dlgRunSvr_.DestroyWindow();
	dlgRunCli_.DestroyWindow();
	dlgUpdate_.DestroyWindow();
}



void CDlgStartupInfo::OnBnClickedOk()
{
	using i8desk::data_helper::BootTaskAreaTraits;

	UpdateData(TRUE);

	if( taskName_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("名字不能为空"));
		msgDlg.DoModal();
		return;
	}

	utility::Strcpy(curBootTask_->Name, (LPCTSTR)taskName_);

	// 检测名字是否重复
	if( i8desk::business::BootTask::HasSameName(curBootTask_) )
	{
		stdex::tString msg = _T("已经存在该名字: ");
		msg += curBootTask_->Name;
		CMessageBox msgDlg(_T("提示"),msg);
		msgDlg.DoModal();
		return;
	}

	// 检测选定区域
	AIDs aids;
	for(HTREEITEM hItem = wndTreeAreas_.GetRootItem(); 
		hItem != NULL; hItem = wndTreeAreas_.GetNextSiblingItem(hItem)) 
	{
		if( wndTreeAreas_.GetCheck(hItem) == TRUE )
		{
			using i8desk::data_helper::AreaTraits;
			typedef AreaTraits::ElementType AreaType;
			AreaType *area = reinterpret_cast<AreaType *>(wndTreeAreas_.GetItemData(hItem));

			aids.insert(area->AID);
		}
	}
	if( aids.empty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("请选择区域"));
		msgDlg.DoModal();
		wndTreeAreas_.SetFocus();
		return;
	}

	// 检测过期日期
	CTime time;
	wndTime_.GetTime(time);
	curBootTask_->EndDate = (i8desk::ulong)time.GetTime();

	// 检测执行日期
	int timeRangeTime = 0;
	for(HTREEITEM hItem = wndTreeTimeRange_.GetRootItem(); 
		hItem != NULL; hItem = wndTreeTimeRange_.GetNextSiblingItem(hItem)) 
	{
		int mask = wndTreeTimeRange_.GetItemData(hItem);
		if( wndTreeTimeRange_.GetCheck(hItem) )
			timeRangeTime |= mask;
	}
	curBootTask_->RunDate = timeRangeTime;

	// 检测时间段
	CTime startTime;
	wndTimeStart_.GetTime(startTime);
	curBootTask_->StartTime = (i8desk::ulong)startTime.GetTime();

	CTime endTime;
	wndTimeEnd_.GetTime(endTime);
	curBootTask_->EndTime = (i8desk::ulong)endTime.GetTime();

	if( startTime > endTime )
	{
		CMessageBox msgDlg(_T("提示"), _T("起始时间不能超过大于时间"));
		msgDlg.DoModal();
		wndTimeStart_.SetFocus();
		return;
	}

	// 检测是否还原保护执行
	curBootTask_->Flag = wndRadioDirectRun_.GetCheck() == BST_CHECKED ? 0 : 1;

	// 检测是否启用
	curBootTask_->Status = wndCheckboxStatus_.GetCheck() == BST_CHECKED;

	// 检测执行时机
	curBootTask_->RunType = i8desk::business::BootTask::SetRunType(wndRadioRunAtBoot_.GetCheck() == BST_CHECKED);

	// 检测延时启动
	CString delayTime;
	wndEditDelayTime_.GetWindowText(delayTime);
	curBootTask_->DelayTime = _ttoi((LPCTSTR)delayTime);

	// 得到任务类型
	curBootTask_->Type = wndComboType_.GetItemData(wndComboType_.GetCurSel());

	// 得到参数
	i8desk::ParamInterface *param = params_[wndMgr_.GetCurrentID()];
	assert(param);

	stdex::tString text;
	if( !param->GetParam(text) )
		return;
	utility::Strcpy(curBootTask_->Parameter, text);

	
	// 数据操作
	BootTaskTraits::MapType &bootTasks = i8desk::GetDataMgr().GetBootTasks();

	if( isAdd_ )
	{
		if( !i8desk::business::BootTask::Add(curBootTask_, aids) )
			return;
	}
	else
	{
		if( !i8desk::business::BootTask::Modify(curBootTask_, aids_, aids) )
			return;
	}


	OnOK();
}

void CDlgStartupInfo::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgStartupInfo::OnCbnSelchangeComboStartupInfoType()
{
	struct
	{
		int type;
		UINT uID;
	}Type2ID[] = 
	{
		{ i8desk::brDeskTop,		CDlgStartupParamDesktop::IDD },
		{ i8desk::brShortcut,		CDlgStartupParamIocn::IDD },
		{ i8desk::brExecSvrFolder,	CDlgStartupParamRunClient::IDD },
		{ i8desk::brSvrExecFile,	CDlgStartupParamRunSvr::IDD },
		{ i8desk::brDelFile,		CDlgStartupParamDel::IDD },
		{ i8desk::brDirectUpdate,	CDlgStartupParamDirectUpdate::IDD },
	};

	int sel = wndComboType_.GetCurSel();
	if( sel == -1 )
		return;
	int bootType = wndComboType_.GetItemData(sel);
	UINT uID = 0;
	for(size_t i = 0; i != _countof(Type2ID); ++i)
	{
		if( Type2ID[i].type == bootType )
		{
			uID = Type2ID[i].uID;
			break;
		}
	}

	assert(uID != 0);
	wndMgr_[uID];
}

BOOL CDlgStartupInfo::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndTreeAreas_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	wndTreeTimeRange_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}

