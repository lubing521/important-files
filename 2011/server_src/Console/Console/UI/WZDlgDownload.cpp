// WZDlgDownload.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgDownload.h"
#include "../ManagerInstance.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/Utility/utility.h"

#include "../CustomControl/CustomListCtrl.h"
#include "../GridListCtrlEx/CGridColumnTraitCombo.h"
#include "../GridListCtrlEx/CGridColumnTraitEdit.h"

#include "../Business/AreaBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../MessageBox.h"

// WZDlgDownload 对话框
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
}


IMPLEMENT_DYNAMIC(WZDlgDownload, CDialog)

WZDlgDownload::WZDlgDownload(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgDownload::IDD, pParent)
	, TempDir_(_T("E:\\msctmp\\"))
	, DownPath_(_T("E:\\"))
{

}

WZDlgDownload::~WZDlgDownload()
{
	outboxLine_.Detach();
}

void WZDlgDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_WZ_DOWNLOADSET_TEMPDIR, TempDir_);
	DDX_Text(pDX, IDC_EDIT_WZ_DOWNLOADSET_AGREEDOWNPATH, DownPath_);

	DDX_Control(pDX, IDC_COMBO_WZ_DOWNLOADSET_MAXTASKNUM, wndComboMaxTaskNum_);
	DDX_Control(pDX, IDC_COMBO_WZ_DOWNLOADSET_MAXSPEED, wndComboMaxSpeed_);
	DDX_Control(pDX, IDC_EDIT_WZ_DOWNLOADSET_TEMPDIR, wndEditTempDir_);
	DDX_Control(pDX, IDC_EDIT_WZ_DOWNLOADSET_AGREEDOWNPATH, wndEditDownPath_);
	DDX_Control(pDX, IDC_BUTTON_WZ_DOWNLOADSET_TEMPDIR, wndBtnTempDir_);
	DDX_Control(pDX, IDC_BUTTON_WZ_DOWNLOADSET_PATH, wndBtnPath_);

	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_SET, wndLabelSet_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_MAXTASKNUM, wndLabelMaxNum_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_MAXSPEED, wndLabelSpeed_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_TEMPDIR, wndLabelDir_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_AGREEDOWNPATH, wndLabelPath_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_MAXSPEED_PARAM, wndLabelParam_);
	DDX_Control(pDX, IDC_STATIC_WZ_DOWNLOADSET_RUN, wndLabelRunType_);
	DDX_Control(pDX, IDC_LIST_WZ_DOWNLOADSET_RUN, listRunTypeCtrl_);


}


BEGIN_MESSAGE_MAP(WZDlgDownload, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_DOWNLOADSET_TEMPDIR, &WZDlgDownload::OnBnClickedSelectMsctmpdir)
	ON_BN_CLICKED(IDC_BUTTON_WZ_DOWNLOADSET_PATH, &WZDlgDownload::OnBnClickedButtonPath)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

static const TCHAR *DownSpeeds[] = 
{
	_T("不限制"),
	_T("80"),
	_T("150"),
	_T("300"),
	_T("550"),
	_T("1100"),
	_T("2000"),
	_T("4000"),
	_T("8200"),
	_T("16500"),
};

static const size_t nDownSpeeds = sizeof(DownSpeeds)/sizeof(TCHAR *);

// WZDlgDownload 消息处理程序
BOOL WZDlgDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	listRunTypeCtrl_.SetHeaderImage(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

	listRunTypeCtrl_.InsertHiddenLabelColumn();
	listRunTypeCtrl_.InsertColumnTrait(1, _T("分区"), LVCFMT_LEFT, 130,1,detail::GetTrait());

	//绘制最大任务数
	wndComboMaxTaskNum_.AddString(_T("1"));
	wndComboMaxTaskNum_.AddString(_T("2"));
	wndComboMaxTaskNum_.AddString(_T("3"));

	//绘制限速
	for (size_t i = 0; i < nDownSpeeds; i++)
		wndComboMaxSpeed_.AddString(DownSpeeds[i]);
	//下载临时目录
	stdex::tString str;
	TempDir_ = i8desk::GetDataMgr().GetOptVal(OPT_D_TEMPDIR,str).c_str();
	//缺省根路径
	DownPath_ = i8desk::GetDataMgr().GetOptVal(OPT_D_INITDIR, str).c_str();
	//选中最大任务数
	int nTaskNum = i8desk::GetDataMgr().GetOptVal(OPT_D_TASKNUM, 3);
	wndComboMaxTaskNum_.SetCurSel(nTaskNum - 1);
	//选中最大速度
	int nValue = i8desk::GetDataMgr().GetOptVal(OPT_D_SPEED, 0);
	if (nValue) 
	{
		for (size_t i = 0; i < nDownSpeeds; i++) 
		{
			if (nValue <= _ttoi(DownSpeeds[i])) 
			{
				wndComboMaxSpeed_.SetCurSel(i);
				break;
			}
		}
	}
	if (wndComboMaxSpeed_.GetCurSel() == -1) 
		wndComboMaxSpeed_.SelectString(0, _T("不限制"));

	UpdateData(FALSE);


	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));

	_ReadDatas();

	return TRUE;
}

void WZDlgDownload::_ReadDatas()
{
	_DrawRunTypeList();

	//绘制运行方式表
	_UpdateRunTypeList();
}

void WZDlgDownload::_DrawRunTypeList()
{
	listRunTypeCtrl_.DeleteAllItems();
	listRunTypeCtrl_.DeleteColumn(2);
	
	//添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	pComboTrait->AddItem(0, i8desk::data_helper::RT_UNKNOWN.c_str());
	pComboTrait->AddItem(1, i8desk::data_helper::RT_LOCAL.c_str());
	pComboTrait->AddItem(2, i8desk::data_helper::RT_DIRECT.c_str());

	int loop = 3;
	i8desk::business::VDisk::RunTypeDesc(std::tr1::bind(&CGridColumnTraitCombo::AddItem, pComboTrait, 
		loop++, std::tr1::placeholders::_1));

	listRunTypeCtrl_.InsertColumnTrait(2, _T("运行方式"), LVCFMT_LEFT, 220, 2, pComboTrait);
}

BOOL WZDlgDownload::_UpdateRunTypeList()
{
	listRunTypeCtrl_.DeleteAllItems();

	stdex::tString szDefRunType = i8desk::GetDataMgr().GetOptVal(OPT_D_AREADEFRUNTYPE, _T(""));
	using i8desk::data_helper::AreaTraits;
	AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);

	int nItem = 0;
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		nItem = listRunTypeCtrl_.InsertItem(nItem, _T(""));
		listRunTypeCtrl_.SetItemText(nItem, 1, iter->second->Name);
		listRunTypeCtrl_.SetItemText(nItem, 2, i8desk::business::Area::ParseRunType(iter->first, szDefRunType).c_str());

		nItem++;
	}

	return TRUE;
}

void WZDlgDownload::OnBnClickedSelectMsctmpdir()
{
	UpdateData(TRUE);

	if (!i8desk::IsRunOnServer())
		AfxMessageBox(_T("请在服务器上设置该功能！"));

	CString selPath;
	if( theApp.GetShellManager()->BrowseForFolder(selPath, this, TempDir_, _T("选择三层临时下载目录:")) )
	{
		if( selPath.IsEmpty() )
			return ;

		if( selPath.Right(1) == _T("\\") )
			selPath.Delete(selPath.GetLength()-1);

		selPath += _T("\\");
		selPath.Trim();

		TempDir_ = selPath;
	}

	UpdateData(FALSE);
}

void WZDlgDownload::OnBnClickedButtonPath()
{
	UpdateData(TRUE);

	if (!i8desk::IsRunOnServer())
		AfxMessageBox(_T("请在服务器上设置该功能！"));

	CString selPath;
	if( theApp.GetShellManager()->BrowseForFolder(selPath, this, DownPath_, _T("选择三层下载目录:")) )
	{
		if( selPath.IsEmpty() )
			return ;

		if( selPath.Right(1) == _T("\\") )
			selPath.Delete(selPath.GetLength()-1);

		selPath += _T("\\");
		selPath.Trim();

		DownPath_ = selPath;
	}

	UpdateData(FALSE);
}

void WZDlgDownload::OnComplate()
{
	//确认键回写数据库
	UpdateData(TRUE);
	i8desk::data_helper::ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();
	//最大任务数
	i8desk::GetDataMgr().SetOptVal(OPT_D_TASKNUM, GetDlgItemInt(IDC_COMBO_WZ_DOWNLOADSET_MAXTASKNUM));
	//最大下载速度
	i8desk::GetDataMgr().SetOptVal(OPT_D_SPEED, GetDlgItemInt(IDC_COMBO_WZ_DOWNLOADSET_MAXSPEED));
	//三层下载临时目录
	i8desk::GetDataMgr().SetOptVal(OPT_D_TEMPDIR, (LPCTSTR)TempDir_);

	i8desk::GetDataMgr().SetOptVal(OPT_D_INITDIR, (LPCTSTR)DownPath_);

	// 区域运行方式
	using i8desk::data_helper::RunTypeTraits;
	stdex::tOstringstream osRunType;
	RunTypeTraits::VectorType runTypes;


	for( int nIdx= 0; nIdx != listRunTypeCtrl_.GetItemCount(); ++nIdx )
	{
		if (!osRunType.str().empty())
			osRunType <<  _T("|");

		RunTypeTraits::ValueType val(new RunTypeTraits::ElementType);

		val->Name = (LPCTSTR)listRunTypeCtrl_.GetItemText(nIdx, 1);
		utility::Strcpy(val->AID, i8desk::business::Area::GetAIDByName(val->Name));
		osRunType << val->AID << _T(",");

		i8desk::business::Area::ParseArea((LPCTSTR)listRunTypeCtrl_.GetItemText(nIdx, 2), val); 

		if( val->Type == i8desk::data_helper::ERT_VDISK )
			osRunType << val->VID;
		else
			osRunType << val->Type;
	}

	i8desk::GetDataMgr().SetOptVal(OPT_D_AREADEFRUNTYPE, osRunType.str().c_str());


}

void WZDlgDownload::OnShow(int showtype)
{
	ShowWindow(showtype);

	UpdateData(TRUE);

	//判断路径是否存在
	if(i8desk::IsRunOnServer())
	{
		if(!PathFileExists((LPCTSTR)TempDir_))
		{
			CMessageBox msgDlg(_T("提示"),_T("三层下载临时目录不存在,请重新设置\n"));
			msgDlg.DoModal();
			return ;
		}

		if(!PathFileExists((LPCTSTR)DownPath_))
		{
			CMessageBox msgDlg(_T("提示"),_T("三层下载缺省根路径不存在,请重新设置\n"));
			msgDlg.DoModal();
			return ;
		}
	}

	_ReadDatas();
}


BOOL WZDlgDownload::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcMultiText;
	listRunTypeCtrl_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, memDC, rcMultiText, &outboxLine_);

	return TRUE;

}