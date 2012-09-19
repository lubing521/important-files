// DlgStartup.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartup.h"


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridColumnTraitText.h"
#include "../Business/BootTaskBusiness.h"
#include "UIHelper.h"
#include "../MessageBox.h"

#include "DlgStartupInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartup

IMPLEMENT_DYNCREATE(CDlgStartup, CDialog)

CDlgStartup::CDlgStartup(CWnd* pParent)
	: i8desk::ui::BaseWnd(CDlgStartup::IDD, pParent)
{

}

CDlgStartup::~CDlgStartup()
{
}

void CDlgStartup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STARTUP_BOOTTASKS, wndListBootTask_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_ADD, wndBtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_DELETE, wndBtnDel_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_MODIFY, wndBtnMod_);

}


BEGIN_EASYSIZE_MAP(CDlgStartup)
	EASYSIZE(IDC_LIST_STARTUP_BOOTTASKS, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)

	EASYSIZE(IDC_BUTTON_STARTUP_ADD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_STARTUP_DELETE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_STARTUP_MODIFY, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

END_EASYSIZE_MAP 



BEGIN_MESSAGE_MAP(CDlgStartup, i8desk::ui::BaseWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_ADD, &CDlgStartup::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_DELETE, &CDlgStartup::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_MODIFY, &CDlgStartup::OnBnClickedButtonStartupModify)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_STARTUP_BOOTTASKS, &CDlgStartup::OnLvnGetdispinfoListStartupBoottasks)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_STARTUP_BOOTTASKS, &CDlgStartup::OnNMRDblclkListStartupBoottasks)

	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgStartup::OnApplySelect)
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
		MASK_BOOT_NAME = 1, 
		MASK_BOOT_TYPE,
		MASK_BOOT_OK, 
		MASK_BOOT_AREA, 
		MASK_BOOT_FLAG, 
		MASK_BOOT_END_DATE, 
		MASK_BOOT_RANGE_DATE, 
		MASK_BOOT_RANGE_TIME,
		MASK_BOOT_RUN_TYPE
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_BOOT_NAME,	_T("名称"),		LVCFMT_LEFT, 70, MASK_BOOT_NAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_TYPE,	_T("类型"),		LVCFMT_LEFT, 140, MASK_BOOT_TYPE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_OK,	_T("启用"),		LVCFMT_LEFT, 50, MASK_BOOT_TYPE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_AREA,	_T("区域"),		LVCFMT_LEFT, 80, MASK_BOOT_AREA,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_FLAG,	_T("任务状态"),	LVCFMT_LEFT, 80, MASK_BOOT_FLAG,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_END_DATE,	_T("过期日期"),	LVCFMT_LEFT, 120, MASK_BOOT_END_DATE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_RANGE_DATE,	_T("执行日期段"),	LVCFMT_LEFT, 200, MASK_BOOT_RANGE_DATE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_RANGE_TIME,	_T("执行时间段"),	LVCFMT_LEFT, 120, MASK_BOOT_RANGE_TIME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_BOOT_RUN_TYPE,		_T("执行类型"),	LVCFMT_LEFT, 136, MASK_BOOT_RUN_TYPE,	detail::GetTrait());
		
		listCtrl.SetItemHeight(24);
	}
}


void CDlgStartup::Register()
{

}

void CDlgStartup::UnRegister()
{

}

void CDlgStartup::OnRealDataUpdate()
{

}


void CDlgStartup::OnReConnect()
{
	i8desk::GetDataMgr().GetBootTasks().clear();
	i8desk::GetDataMgr().GetBootTaskAreas().clear();
	i8desk::GetDataMgr().GetAreas().clear();
}

void CDlgStartup::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTasks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTaskAreas());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetAreas());
}

void CDlgStartup::OnDataComplate()
{
	_ShowView();
}


void CDlgStartup::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::BootTaskTraits;

	bootTasks_.clear();
	const BootTaskTraits::MapType &tasks = i8desk::GetDataMgr().GetBootTasks();

	stdex::map2vector(tasks, bootTasks_);

	size_t count = bootTasks_.size();
	wndListBootTask_.SetItemCount(count);
}



// CDlgStartup 消息处理程序

BOOL CDlgStartup::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;


	// 初始化List
	InitListCtrl(wndListBootTask_);
	wndListBootTask_.SetItemHeight(24);


	wndListBootTask_.SetHeaderImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));


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


	workOutLine_.reset(new CImage);

	workOutLine_->Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));

	// 初始化Button
	HBITMAP add[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Press.png"))
	};
	wndBtnAdd_.SetImages(add[0], add[1], add[2]);

	HBITMAP del[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Press.png"))
	};
	wndBtnDel_.SetImages(del[0], del[1], del[2]);

	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndBtnMod_.SetImages(modify[0], modify[1], modify[2]);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgStartup::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, area_);

	CRect rcList;
	wndListBootTask_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, workOutLine_);

}

void CDlgStartup::OnDestroy()
{
	CDialog::OnDestroy();

}


void CDlgStartup::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	if( ::IsWindow(wndListBootTask_.GetSafeHwnd()) )
		wndListBootTask_.Invalidate();
}


LRESULT CDlgStartup::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 )
	{
		
	}
	else
	{

	}
	return TRUE;
}

void CDlgStartup::OnLvnGetdispinfoListStartupBoottasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= bootTasks_.size() )
		return;

	const i8desk::data_helper::BootTaskTraits::ValueType &val = bootTasks_[itemIndex];


	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_BOOT_NAME:
			text = val->Name;
			break;
		case MASK_BOOT_OK:
			text = val->Status == 1 ? _T("启用") : _T("未启用");
			break;
		case MASK_BOOT_RANGE_DATE:
			text = i8desk::business::BootTask::GetRangeDate(val);
			break;
		case MASK_BOOT_RANGE_TIME:
			text = i8desk::business::BootTask::GetRangeTime(val);
			break;
		case MASK_BOOT_RUN_TYPE:
			text = i8desk::business::BootTask::GetRunType(val);
			break;
		case MASK_BOOT_AREA:
			text = i8desk::business::BootTask::GetAreaDesc(val);
			break;
		case MASK_BOOT_END_DATE:
			text = i8desk::FormatTime(val->EndDate);
			break;
		case MASK_BOOT_FLAG:
			text = val->Flag == 0 ? _T("固定执行") : _T("仅在还原保护执行");
			break;
		case MASK_BOOT_TYPE:
			text = i8desk::business::BootTask::GetTypeDesc(val);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgStartup::OnBnClickedDelete()
{
	if( wndListBootTask_.GetSelectedCount() == 0 )
		return;
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的开机任务吗?"));

	if( msgDlg.DoModal() == IDCANCEL)
		return;

	int nSel = -1;


	typedef std::vector<DWORD> SelBootTask;
	SelBootTask vecIdx;
	while((nSel = wndListBootTask_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		vecIdx.push_back(nSel);
	}

	for(SelBootTask::const_iterator rit = vecIdx.begin(); rit != vecIdx.end(); ++rit)
	{
		nSel = *rit;
		if( !i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetBootTasks(), bootTasks_[nSel]->TID) )
			break;
	}

	_ShowView();

}

void CDlgStartup::OnBnClickedAdd()
{
	CDlgStartupInfo dlg(true, 0);
	if( dlg.DoModal() == IDOK )
	{
		_ShowView();
	}
}


void CDlgStartup::OnBnClickedButtonStartupModify()
{
	int nSel = wndListBootTask_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;
	
	assert(nSel < (int)bootTasks_.size() );
	
	CDlgStartupInfo dlg(false, bootTasks_[nSel].get());
	if( dlg.DoModal() == IDOK )
	{
		_ShowView();
	}
}


void CDlgStartup::OnNMRDblclkListStartupBoottasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	OnBnClickedButtonStartupModify();
}
