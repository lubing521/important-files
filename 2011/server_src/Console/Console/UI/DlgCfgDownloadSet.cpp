#include "stdafx.h"
#include "../Console.h"
#include "DlgCfgDownloadSet.h"
#include "DlgCfgSetUptTime.h"
#include "DlgCfgBrowseDir.h"
#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridColumnTraitCombo.h"
#include "../GridListCtrlEx/CGridColumnTraitEdit.h"
#include "../CustomControl/CustomCheckBox.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../MessageBox.h"

#include "../Data/CustomDataType.h"

#include "../Business/AreaBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/ConfigBusiness.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



namespace
{
	bool operator==(const CString &lhs, const stdex::tString &rhs)
	{
		return (LPCTSTR)(lhs) == rhs;
	}
}

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

// CDlgOptDownloadSet 对话框

IMPLEMENT_DYNAMIC(CDlgCfgDownloadSet, CDialog)

CDlgCfgDownloadSet::CDlgCfgDownloadSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfgDownloadSet::IDD, pParent)
	, bModifyClass_(FALSE)
	, tempdir_(_T("E:\\msctmp\\"))
	, downpath_(_T("E:\\"))
{

}

CDlgCfgDownloadSet::~CDlgCfgDownloadSet()
{
}

void CDlgCfgDownloadSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CFGDOWNLOADSET_TIME, listTimeCtrl_);
	DDX_Control(pDX, IDC_CHECK_CFGDOWNLOADSET_TIME, wndCheckTimeUpt_);
	DDX_Control(pDX, IDC_LIST_CFGDOWNLOADSET_RUN, listRunTypeCtrl_);
	DDX_Control(pDX, IDC_COMBO_CFGDOWNLOADSET_MAXTASKNUM, wndComboTaskNumCtrl_);
	DDX_Control(pDX, IDC_COMBO_CFGDOWNLOADSET_MAXSPEED, wndComboDownSpeed_);

	DDX_Control(pDX, IDC_EDIT_CFGDOWNLOADSET_TEMPDIR, wndEditTempDir_);
	DDX_Text(pDX, IDC_EDIT_CFGDOWNLOADSET_TEMPDIR, tempdir_);
	DDX_Control(pDX, IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, wndEditDownPath_);
	DDX_Text(pDX, IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, downpath_);

	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_SET, wndSet_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_MAXTASKNUM, wndMaxTaskNum_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_MAXSPEED, wndMaxSpeed_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_MAXSPEED_PARAM, wndMaxSpeedParam_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_TEMPDIR, wndTempDir_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_DOWNPATH, wndDownPath_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_AGREEDOWNPATH, wndAgreeDownPath_);
	DDX_Control(pDX, IDC_STATIC_CFGDOWNLOADSET_RUN, wndRun_);

	DDX_Control(pDX, IDC_BUTTON_CFGDOWNLOADSET_ADD, wndBtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_CFGDOWNLOADSET_DEL, wndBtnDel_);
	DDX_Control(pDX, IDC_BUTTON_CFGDOWNLOADSET_TEMPDIR, wndBtnTempDir_);
	DDX_Control(pDX, IDC_BUTTON_CFGDOWNLOADSET_PATH, wndBtnPath_);

}

BEGIN_EASYSIZE_MAP(CDlgCfgDownloadSet)   
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_SET, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_MAXTASKNUM, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_MAXSPEED, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_TEMPDIR, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_DOWNPATH, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_AGREEDOWNPATH, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_CHECK_CFGDOWNLOADSET_TIME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_CFGDOWNLOADSET_TIME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	
	EASYSIZE(IDC_BUTTON_CFGDOWNLOADSET_ADD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CFGDOWNLOADSET_DEL, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_COMBO_CFGDOWNLOADSET_MAXTASKNUM, IDC_STATIC_CFGDOWNLOADSET_MAXTASKNUM, ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_COMBO_CFGDOWNLOADSET_MAXSPEED, IDC_STATIC_CFGDOWNLOADSET_MAXSPEED,ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_EDIT_CFGDOWNLOADSET_TEMPDIR, IDC_STATIC_CFGDOWNLOADSET_TEMPDIR,ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, IDC_STATIC_CFGDOWNLOADSET_AGREEDOWNPATH, ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_STATIC_CFGDOWNLOADSET_RUN, ES_BORDER, ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_CFGDOWNLOADSET_RUN, ES_BORDER, ES_BORDER,  ES_KEEPSIZE, ES_KEEPSIZE, 0)


END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgCfgDownloadSet, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_CHECK_CFGDOWNLOADSET_TIME, &CDlgCfgDownloadSet::OnBnClickedCheckTimeupt)
	ON_BN_CLICKED(IDC_BUTTON_CFGDOWNLOADSET_ADD, &CDlgCfgDownloadSet::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON_CFGDOWNLOADSET_DEL, &CDlgCfgDownloadSet::OnBnClickedDel)

	ON_BN_CLICKED(IDC_BUTTON_CFGDOWNLOADSET_TEMPDIR, &CDlgCfgDownloadSet::OnBnClickedSelectMsctmpdir)
	ON_BN_CLICKED(IDC_BUTTON_CFGDOWNLOADSET_PATH, &CDlgCfgDownloadSet::OnBnClickedButtonPath)
	ON_MESSAGE(WM_MSG_OK, &CDlgCfgDownloadSet::OnApplyMessage)
	//ON_EN_CHANGE(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, &CDlgCfgDownloadSet::OnEnChangeAgreeDownPath)
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

BOOL CDlgCfgDownloadSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClasses());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetAreas());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetRunTypes());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());

	wndComboTaskNumCtrl_.LimitText(2);
	wndComboDownSpeed_.LimitText(4);

	//绘制最大任务数
	wndComboTaskNumCtrl_.AddString(_T("1"));
	wndComboTaskNumCtrl_.AddString(_T("2"));
	wndComboTaskNumCtrl_.AddString(_T("3"));

	//绘制限速
	for (size_t i = 0; i < nDownSpeeds; i++)
		wndComboDownSpeed_.AddString(DownSpeeds[i]);

	//绘制时段更新表

	listTimeCtrl_.SetHeaderImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

	listTimeCtrl_.InsertHiddenLabelColumn();

	listTimeCtrl_.InsertColumnTrait(1, _T("开始时间"), LVCFMT_LEFT, 80,1,detail::GetTrait());
	listTimeCtrl_.InsertColumnTrait(2, _T("停止时间"), LVCFMT_LEFT, 80,2,detail::GetTrait());

	//绘制运行方式表
	DrawRunTypeList();
	//读取选项表数据
	ReadData();

	UpdateData(FALSE);

	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));

	return TRUE; 
}

void CDlgCfgDownloadSet::DrawRunTypeList()
{
	//绘制运行方式表
	listRunTypeCtrl_.SetHeaderImage(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));


	listRunTypeCtrl_.InsertHiddenLabelColumn();
	listRunTypeCtrl_.InsertColumnTrait(1, _T("分区"), LVCFMT_LEFT, 80,1,detail::GetTrait());
	//添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	pComboTrait->AddItem(0, i8desk::data_helper::RT_UNKNOWN.c_str());
	pComboTrait->AddItem(1, i8desk::data_helper::RT_LOCAL.c_str());
	pComboTrait->AddItem(2, i8desk::data_helper::RT_DIRECT.c_str());

	int loop = 3;
	i8desk::business::VDisk::RunTypeDesc(std::tr1::bind(&CGridColumnTraitCombo::AddItem, pComboTrait, 
		loop++, std::tr1::placeholders::_1));

	listRunTypeCtrl_.InsertColumnTrait(2, _T("运行方式"), LVCFMT_LEFT, 140, 2, pComboTrait);
}

void CDlgCfgDownloadSet::ReadData()
{
	//下载临时目录
	stdex::tString str;
	tempdir_ = i8desk::GetDataMgr().GetOptVal(OPT_D_TEMPDIR,str).c_str();
	//缺省根路径
	downpath_ = i8desk::GetDataMgr().GetOptVal(OPT_D_INITDIR, str).c_str();
	//选中最大任务数
	int nTaskNum = i8desk::GetDataMgr().GetOptVal(OPT_D_TASKNUM, 3);
	wndComboTaskNumCtrl_.SetCurSel(nTaskNum - 1);
	//选中最大速度
	int nValue = i8desk::GetDataMgr().GetOptVal(OPT_D_SPEED, 0);
	if (nValue) 
	{
		for (size_t i = 0; i < nDownSpeeds; i++) 
		{
			if (nValue <= _ttoi(DownSpeeds[i])) 
			{
				wndComboDownSpeed_.SetCurSel(i);
				break;
			}
		}
	}
	if (wndComboDownSpeed_.GetCurSel() == -1) 
	{
		wndComboDownSpeed_.SelectString(0, _T("不限制"));
	}

	//读取时段更新数据
	int ischeck = i8desk::GetDataMgr().GetOptVal(OPT_D_TIMEUPT, 0);
	wndCheckTimeUpt_.SetCheck(ischeck);
	str = i8desk::GetDataMgr().GetOptVal(OPT_D_TIMEUPTVALUE, str);
	listTimeCtrl_.DeleteAllItems();
	for (int nIdx=0; ; nIdx++)
	{
		CString szItem, szStart, szEnd;
		AfxExtractSubString(szItem, str.c_str(), nIdx, '|');
		if (szItem.IsEmpty())
			break;

		AfxExtractSubString(szStart, szItem, 0, ',');
		AfxExtractSubString(szEnd, szItem, 1, ',');
		if (!szStart.IsEmpty() && !szEnd.IsEmpty())
		{
			int nItem = listTimeCtrl_.InsertItem(listTimeCtrl_.GetItemCount(), szStart);
			listTimeCtrl_.SetItemText(nItem, 1, szStart);
			listTimeCtrl_.SetItemText(nItem, 2, szEnd);
		}
	}

	UpdateRunTypeList();

	OnBnClickedCheckTimeupt();

	UpdateData(FALSE);
}


BOOL CDlgCfgDownloadSet::UpdateRunTypeList()
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


void CDlgCfgDownloadSet::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

}

BOOL CDlgCfgDownloadSet::OnEraseBkgnd(CDC *pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(*pDC, rcClient);
	memDC.GetDC().FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		RGB(255, 255, 255));

	CRect rcMultiText;
	listRunTypeCtrl_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, memDC, rcMultiText, &outboxLine_);
	listTimeCtrl_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, memDC, rcMultiText, &outboxLine_);

	return TRUE;
}

void CDlgCfgDownloadSet::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}
// CDlgOptDownloadSet 消息处理程序
void CDlgCfgDownloadSet::OnBnClickedCheckTimeupt()
{
	BOOL bEnable = (wndCheckTimeUpt_.GetCheck() == BST_CHECKED);
	listTimeCtrl_.EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CFGDOWNLOADSET_ADD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CFGDOWNLOADSET_DEL)->EnableWindow(bEnable);
}

void CDlgCfgDownloadSet::OnBnClickedAdd()
{
	CDlgCfgSetUptTime dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString szStart, szEnd;
		szStart.Format(_T("%02d:%02d"), dlg.tmStart.GetHour(), dlg.tmStart.GetMinute());
		szEnd.Format(_T("%02d:%02d"), dlg.tmEnd.GetHour(), dlg.tmEnd.GetMinute());
		int nIdx = listTimeCtrl_.InsertItem(listTimeCtrl_.GetItemCount(), _T(""));
		listTimeCtrl_.SetItemText(nIdx, 1, szStart);
		listTimeCtrl_.SetItemText(nIdx, 2, szEnd);
	}
}

void CDlgCfgDownloadSet::OnBnClickedDel()
{
	int nSel = listTimeCtrl_.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;
	listTimeCtrl_.DeleteItem(nSel);
}

void CDlgCfgDownloadSet::OnBnClickedButtonPath()
{
	UpdateData(TRUE);
	if (i8desk::IsRunOnServer())
	{
		CString selPath;
		if( theApp.GetShellManager()->BrowseForFolder(selPath, this, downpath_, _T("选择三层下载目录:")) )
		{
			if( selPath.IsEmpty() )
				return ;

			if( selPath.Right(1) == _T("\\") )
				selPath.Delete(selPath.GetLength()-1);

			selPath += _T("\\");
			selPath.Trim();

			downpath_ = selPath;
		}
	}
	else
	{
		CDlgCfgBrowseDir dlg;
		GetDlgItemText(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, dlg.strDirOrFile_);
		if (dlg.DoModal() != IDOK)
			return;
		SetDlgItemText(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, dlg.strDirOrFile_);
	}

	UpdateData(FALSE);
}

void CDlgCfgDownloadSet::OnBnClickedSelectMsctmpdir()
{
	UpdateData(TRUE);

	if (i8desk::IsRunOnServer())
	{
		CString selPath;
		if( theApp.GetShellManager()->BrowseForFolder(selPath, this, tempdir_, _T("选择三层临时下载目录:")) )
		{
			if( selPath.IsEmpty() )
				return ;

			if( selPath.Right(1) == _T("\\") )
				selPath.Delete(selPath.GetLength()-1);

			selPath += _T("\\");
			selPath.Trim();

			tempdir_ = selPath;
		}
	}
	else
	{
		CDlgCfgBrowseDir dlg;
		 dlg.strDirOrFile_ = tempdir_;
		if (dlg.DoModal() != IDOK)
			return;
		 tempdir_ = dlg.strDirOrFile_;
	}

	UpdateData(FALSE);
}

LRESULT CDlgCfgDownloadSet::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	//确认键回写数据库
	UpdateData(TRUE);
	i8desk::data_helper::ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();
	//最大任务数
	i8desk::GetDataMgr().SetOptVal(OPT_D_TASKNUM, GetDlgItemInt(IDC_COMBO_CFGDOWNLOADSET_MAXTASKNUM));
	//最大下载速度
	i8desk::GetDataMgr().SetOptVal(OPT_D_SPEED, GetDlgItemInt(IDC_COMBO_CFGDOWNLOADSET_MAXSPEED));

	//判断盘符是否存在，盘符不存在则提示，存在则看路径是否存在，路径不存在则创建
	if(i8desk::IsRunOnServer())
	{	
		if(!i8desk::business::config::IsValidPath((LPCTSTR)tempdir_))
		{
			CMessageBox msgDlg(_T("提示"),_T("三层下载临时目录盘符不存在或者文件夹创建不成功\n"));
			msgDlg.DoModal();
			return FALSE;
		}

		if(!i8desk::business::config::IsValidPath((LPCTSTR)downpath_))
		{
			CMessageBox msgDlg(_T("提示"),_T("缺省根目录盘符不存在或者文件夹创建不成功\n"));
			msgDlg.DoModal();
			return FALSE;
		}
	}

	//三层下载临时目录
	i8desk::GetDataMgr().SetOptVal(OPT_D_TEMPDIR, (LPCTSTR)tempdir_);

	i8desk::GetDataMgr().SetOptVal(OPT_D_INITDIR, (LPCTSTR)downpath_);

	//启用时段更新
	i8desk::GetDataMgr().SetOptVal(OPT_D_TIMEUPT, wndCheckTimeUpt_.GetCheck());
	//时间值
	CString szValue;
	for (int nIdx=0; nIdx<listTimeCtrl_.GetItemCount(); nIdx++)
	{
		if (!szValue.IsEmpty())
			szValue += _T("|");
		szValue += listTimeCtrl_.GetItemText(nIdx, 1);
		szValue += _T(",");
		szValue += listTimeCtrl_.GetItemText(nIdx, 2);
	}
	i8desk::GetDataMgr().SetOptVal(OPT_D_TIMEUPTVALUE, (LPCTSTR)szValue);

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

	return TRUE;

}

