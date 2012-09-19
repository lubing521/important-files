// DlgVDiskMgr.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgVDiskMgr.h"


#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/ServerBusiness.h"
#include "../DlgWaitMsg.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"
#include "../Misc.h"
#include "WindowManager.h"
#include <bitset>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
		MASK_VDISK_SVRNAME = 1, MASK_VDISK_IP, MASK_VDISK_SVRDRV, MASK_VDISK_CLIDRV, MASK_VDISK_REFESHTIME,  MASK_VDISK_SIZE, MASK_VDISK_SSDDRV
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_VDISK_SVRNAME,		_T("服务器名称"),	LVCFMT_LEFT, 100, MASK_VDISK_SVRNAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_IP,			_T("服务器IP"),	LVCFMT_LEFT, 100, MASK_VDISK_IP,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_SVRDRV,		_T("服务器盘符"),	LVCFMT_LEFT, 80, MASK_VDISK_SVRDRV,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_CLIDRV,		_T("客户机盘符"),	LVCFMT_LEFT, 80, MASK_VDISK_CLIDRV,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_REFESHTIME,	_T("刷盘时机"),	LVCFMT_LEFT, 80, MASK_VDISK_REFESHTIME, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_SIZE,			_T("缓存大小"),	LVCFMT_LEFT, 80, MASK_VDISK_SIZE, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISK_SSDDRV,		_T("SSD盘符"),		LVCFMT_LEFT, 80, MASK_VDISK_SSDDRV, detail::GetTrait());

	}

	struct AddSvrCombo
	{
		CComboBox &combo_;
		AddSvrCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(LPCTSTR name, LPCTSTR sid)
		{
			int index = combo_.AddString(name);
			combo_.SetItemData(index, reinterpret_cast<DWORD_PTR>(sid));
		}
	};

	struct AddCombo
	{
		CComboBox &combo_;
		AddCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(int type, LPCTSTR desc)
		{
			int index = combo_.AddString(desc);
			combo_.SetItemData(index, type);
		}
	};

	std::bitset<MASK_VDISK_SSDDRV + 1> VDiskCompare;
	using namespace i8desk::business;

	struct Compare
	{
		int col_;
		bool ascSort_;

		Compare(int col, bool ascSort)
			: col_(col)
			, ascSort_(ascSort)
		{}

		template <typename T>
		bool operator()(const T &lhs, const T &rhs) const
		{
			switch(col_)
			{
			case MASK_VDISK_SVRNAME:
				
				return ascSort_ ? utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) > 0 :
					utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) < 0;
			case MASK_VDISK_IP:
				return ascSort_ ? lhs->SoucIP > rhs->SoucIP : lhs->SoucIP < rhs->SoucIP;
			case MASK_VDISK_SVRDRV:
				return ascSort_ ? lhs->SvrDrv > rhs->SvrDrv : lhs->SvrDrv < rhs->SvrDrv;
			case MASK_VDISK_CLIDRV:
				return ascSort_ ? lhs->CliDrv > rhs->CliDrv : lhs->CliDrv < rhs->CliDrv;
			case MASK_VDISK_REFESHTIME:
				return ascSort_ ? lhs->LoadType > rhs->LoadType : lhs->LoadType < rhs->LoadType;
			case MASK_VDISK_SIZE:
				return ascSort_ ? lhs->Size > rhs->Size : lhs->Size < rhs->Size;
			case MASK_VDISK_SSDDRV:
				return ascSort_ ? lhs->SsdDrv > rhs->SsdDrv : lhs->SsdDrv < rhs->SsdDrv;
			default:
				assert(0);
				return false;
			}
		}
	};


}


// CDlgVDiskMgr 对话框

IMPLEMENT_DYNAMIC(CDlgVDiskMgr, CNonFrameChildDlg)

CDlgVDiskMgr::CDlgVDiskMgr(CWnd* pParent)
	: CNonFrameChildDlg(CDlgVDiskMgr::IDD, pParent)
	, msg_(_T(""))
	, isAdd_(false)
	, parent_(pParent)
	, textSize_(_T(""))
{
}

CDlgVDiskMgr::~CDlgVDiskMgr()
{
}

void CDlgVDiskMgr::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VDISK_SVRNAME, wndComboSvrName_);
	DDX_Control(pDX, IDC_COMBO_VDISK_SVRIP, wndComboSvrIP_);
	DDX_Control(pDX, IDC_COMBO_VDISK_SVRDRV, wndComboServerDriver_);
	DDX_Control(pDX, IDC_COMBO_VDISK_CLIDRV, wndComboClientDriver_);
	DDX_Control(pDX, IDC_COMBO_VDISK_LOADTYPE, wndComboRefreshTime_);
	DDX_Control(pDX, IDC_COMBO_VDISK_SSDDRV, wndComboSsdDriver_);


	DDX_Control(pDX, IDC_LIST_VDISK_INFO, wndListVDisks_);
	DDX_Text(pDX, IDC_EDIT_VDISK_MSG, msg_);
	DDX_Control(pDX, IDC_EDIT_VDISK_MSG, wndEditmsg_);
	DDX_Text(pDX, IDC_EDIT_VDISK_SIZE, textSize_);
	DDX_Control(pDX, IDC_EDIT_VDISK_SIZE, wndEditSize_);

	DDX_Control(pDX, IDC_STATIC_VDISK_SET, wndVDisksSet_);
	DDX_Control(pDX, IDC_STATIC_VDISK_IP, wndVDisksIP_);
	DDX_Control(pDX, IDC_STATIC_VDISK_SVRNAME, wndSvrName_);

	DDX_Control(pDX, IDC_STATIC_VDISK_SVRDRV, wndServerDriver_);
	DDX_Control(pDX, IDC_STATIC_VDISK_CLIDRV, wndClientDriver_);
	DDX_Control(pDX, IDC_STATIC_VDISK_LOADTYPE, wndRefreshTime_);
	DDX_Control(pDX, IDC_STATIC_VDISK_INFO, wndVDisksInfo_);
	DDX_Control(pDX, IDC_STATIC_VDISK_LIST, wndVDisksList_);
	DDX_Control(pDX, IDC_STATIC_VDISK_SIZE, wndSize_);
	DDX_Control(pDX, IDC_STATIC_VDISK_MB, wndMB_);
	DDX_Control(pDX, IDC_STATIC_VDISK_SSDDRV, wndSsdDriver_);


	DDX_Control(pDX, IDC_BUTTON_VDISK_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_VDISK_MODIFY, wndbtnModify_);
	DDX_Control(pDX, IDC_BUTTON_VDISK_DELETE, wndbtnDelete_);
	DDX_Control(pDX, IDC_BUTTON_VDISK_CHECK, wndbtnCheck_);
	DDX_Control(pDX, IDC_BUTTON_VDISK_REFLESH, wndbtnReflish_);
	DDX_Control(pDX, IDC_BUTTON_VDISK_REFLESH_ALL, wndbtnReflishAll_);

}


BEGIN_MESSAGE_MAP(CDlgVDiskMgr, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_ADD, &CDlgVDiskMgr::OnBnClickedButtonVdiskAdd)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_MODIFY, &CDlgVDiskMgr::OnBnClickedButtonVdiskModify)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_DELETE, &CDlgVDiskMgr::OnBnClickedButtonVdiskDelete)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_CHECK, &CDlgVDiskMgr::OnBnClickedButtonVdiskCheck)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_REFLESH, &CDlgVDiskMgr::OnBnClickedButtonVdiskReflesh)
	ON_BN_CLICKED(IDC_BUTTON_VDISK_REFLESH_ALL, &CDlgVDiskMgr::OnBnClickedButtonVdiskRefleshAll)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VDISK_INFO, &CDlgVDiskMgr::OnNMClickListVdiskInfo)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_VDISK_INFO, &CDlgVDiskMgr::OnLvnDeleteitemListVdiskInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VDISK_INFO, &CDlgVDiskMgr::OnLvnItemchangedListVdiskInfo)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_VDISK_INFO, &CDlgVDiskMgr::OnLvnColumnclickListVdiskInfo)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_VDISK_INFO, &CDlgVDiskMgr::OnLvnGetdispinfoListVdiskInfo)
	ON_CBN_SELCHANGE(IDC_COMBO_VDISK_SVRNAME, &CDlgVDiskMgr::OnCbnSelchangeComboSvrName)

END_MESSAGE_MAP()





namespace 
{
	template < typename StringT, typename MsgT, typename DlgT >
	void AddMsg(StringT &msg, const MsgT &info, DlgT &dlg)
	{
		msg += info;
		msg += _T("\r\n");

		dlg->UpdateData(FALSE);
	}
}


// CDlgVDiskMgr 消息处理程序

BOOL CDlgVDiskMgr::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("虚拟磁盘管理"));

	// Out Line
	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	i8desk::business::Server::GetAllService(AddSvrCombo(wndComboSvrName_));
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
	OnCbnSelchangeComboSvrName();

	for(TCHAR chDrv='C'; chDrv<='Z'; ++chDrv)
	{
		CString text;
		text += chDrv;
		wndComboServerDriver_.AddString(text);
		wndComboClientDriver_.AddString(text);
	}

	wndComboServerDriver_.SetCurSel(0);
	wndComboClientDriver_.SetCurSel(0);

	
	i8desk::business::VDisk::GetLoadTypes(AddCombo(wndComboRefreshTime_));
	wndComboRefreshTime_.SetCurSel(0);

	i8desk::business::VDisk::GetSsdDrv(AddCombo(wndComboSsdDriver_));
	wndComboSsdDriver_.SetCurSel(0);


	InitListCtrl(wndListVDisks_);

	_ReadVdisk();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgVDiskMgr::_ReadVdisk()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	VDiskTraits::MapType &vDisks = i8desk::GetDataMgr().GetVDisks();

	stdex::map2vector(vDisks, curvDisks_);

	if(curvDisks_.empty())
		return;

	TCHAR svrDrv[] = {curvDisks_[0]->SvrDrv, 0};
	wndComboServerDriver_.SelectString(-1, svrDrv);

	TCHAR cliDrv[] = {curvDisks_[0]->CliDrv, 0};
	wndComboClientDriver_.SelectString(-1, cliDrv);

	wndComboRefreshTime_.SelectString(-1, i8desk::business::VDisk::GetLoadTypeDescByType(curvDisks_[0]->LoadType));
	wndComboSsdDriver_.SelectString(-1, i8desk::business::VDisk::GetSsdDescByType(curvDisks_[0]->SsdDrv));


	stdex::tString size;
	stdex::ToString(curvDisks_[0]->Size, size);
	textSize_ = size.c_str();

	_ShowView();

	UpdateData(FALSE);
}

bool CDlgVDiskMgr::_CheckDatas(i8desk::data_helper::VDiskTraits::ValueType &val)
{
	UpdateData(TRUE);

	using namespace i8desk::data_helper;
	int SvrDrv = 0;
	int CliDrv = 0;

	val->Type = I8VDisk;
	CString IP;
	wndComboSvrIP_.GetWindowText(IP);
	
	SvrDrv = wndComboServerDriver_.GetCurSel() + 'C';
	val->Port = i8desk::business::VDisk::VDiskPort + (SvrDrv - 'A');
	val->LoadType = wndComboRefreshTime_.GetItemData(wndComboRefreshTime_.GetCurSel());
	utility::Strcpy(val->SvrMode, MAIN_SERVER_SID);

	CliDrv = wndComboClientDriver_.GetCurSel() + 'C';
	val->SsdDrv = wndComboSsdDriver_.GetItemData(wndComboSsdDriver_.GetCurSel());


	if( utility::IsEmpty((LPCTSTR)IP) || !i8desk::IsValidIP((LPCTSTR)IP) )
	{
		CMessageBox msgDlg(_T("提示"),_T("IP地址不正确!"));
		msgDlg.DoModal();
		return false;
	}
	val->SoucIP = ::htonl(i8desk::String2IP((LPCTSTR)IP));
	
	stdex::tString svrID = i8desk::business::Server::GetServerIDBySvrIP(val->SoucIP);
	if( svrID.empty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("没有该IP的服务器!"));
		msgDlg.DoModal();
		return false;
	}
	else
		utility::Strcpy(val->SvrID, svrID);

	struct ErrorMsg
	{
		void operator()(LPCTSTR tip, LPCTSTR info)
		{
			CMessageBox box(tip, info);
			box.DoModal();
		}
	};
	
	if( !i8desk::business::VDisk::IsValid(val->VID, val->SoucIP, SvrDrv, CliDrv, ErrorMsg()) )
	{
		return false;
	}

	val->SvrDrv = SvrDrv;
	val->CliDrv = CliDrv;

	stdex::tString size = textSize_;
	stdex::ToNumber(val->Size, size);
	
	return true;
}


void CDlgVDiskMgr::_VDiskOperateOne(int nSel, const Callback &callback)
{
	i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

	std::string ip = CT2A(i8desk::IP2String(val->SoucIP).c_str());
	u_short port = i8desk::business::VDisk::VDiskPort + val->SvrDrv - 'A';

	CString msg;
	msg.Format(_T("测试[%s:%C]"), i8desk::IP2String(val->SoucIP).c_str(), port - i8desk::business::VDisk::VDiskPort + 'A');
	msg_ += msg;

	CDlgWaitMsg wait(10);
	wait.Exec(std::tr1::bind(callback, std::tr1::cref(ip), port, std::tr1::ref(msg_)));

	AddMsg(msg_, _T(""), this);
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskAdd()
{
	using namespace i8desk::data_helper;
	VDiskTraits::ValueType val(new VDiskTraits::ElementType);
	utility::Strcpy(val->VID, utility::CreateGuidString());
	isAdd_ = true;
	if( !_CheckDatas(val) )
		return;

	VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

	if( i8desk::GetDataMgr().AddData(VDisks, val->VID, val) )
	{
		parent_->SendMessage(i8desk::ui::WM_ADD_VDISK_MSG, reinterpret_cast<WPARAM>(val.get()));
		curvDisks_.push_back(val);
		_ShowView();
	}
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskModify()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return ;
	isAdd_ = false;
	i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

	if( !_CheckDatas(val) )
		return;

	i8desk::data_helper::VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

	if( i8desk::GetDataMgr().ModifyData(VDisks, val->VID, val) )
	{
		_ShowView();
	}
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskDelete()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的项吗?"));

	if( nSel != -1 && msgDlg.DoModal() == IDOK )
	{
		i8desk::data_helper::VDiskTraits::ValueType val = curvDisks_[nSel];
		i8desk::data_helper::VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

		if( i8desk::GetDataMgr().DelData(VDisks, val->VID) )
		{
			curvDisks_.erase(curvDisks_.begin() + nSel);
			_ShowView();
		}
	}
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskCheck()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
	{
		CMessageBox msgDlg(_T("提示"),_T("请选中要检测的虚拟盘."));
		msgDlg.DoModal();
		return ;
	}

	msg_ = _T("");
	UpdateData(FALSE);

	using namespace std::tr1::placeholders;
	_VDiskOperateOne(nSel, std::tr1::bind(&i8desk::business::VDisk::Check, _1, _2, _3));
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskReflesh()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 || curvDisks_[nSel]->Type != i8desk::data_helper::I8VDisk )
	{
		CMessageBox msgDlg(_T("提示"),_T("请选中要检测的虚拟盘."));
		msgDlg.DoModal();
		return ;
	}

	msg_ = _T("");
	UpdateData(FALSE);

	using namespace std::tr1::placeholders;
	_VDiskOperateOne(nSel, std::tr1::bind(&i8desk::business::VDisk::Refresh, _1, _2, _3));
}

void CDlgVDiskMgr::OnBnClickedButtonVdiskRefleshAll()
{
	msg_ = _T("");
	UpdateData(FALSE);

	for(int nSel = 0; nSel != curvDisks_.size(); ++nSel) 
	{
		using namespace std::tr1::placeholders;
		if( curvDisks_[nSel]->Type == i8desk::data_helper::OtherVDisk )
			continue;
		_VDiskOperateOne(nSel, std::tr1::bind(&i8desk::business::VDisk::Refresh, _1, _2, _3));
	}
}



void CDlgVDiskMgr::OnNMClickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	
	POSITION pos = wndListVDisks_.GetFirstSelectedItemPosition();
	BOOL b = pos != NULL;

	wndbtnModify_.EnableWindow(b);
	wndbtnDelete_.EnableWindow(b);
	wndbtnCheck_.EnableWindow(b);
	wndbtnReflish_.EnableWindow(b);
}

void CDlgVDiskMgr::OnLvnDeleteitemListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	POSITION pos = wndListVDisks_.GetFirstSelectedItemPosition();
	BOOL b = pos != NULL;

	wndbtnModify_.EnableWindow(b);
	wndbtnDelete_.EnableWindow(b);
	wndbtnCheck_.EnableWindow(b);
	wndbtnReflish_.EnableWindow(b);
}

void CDlgVDiskMgr::OnLvnItemchangedListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 ) 
		return ;

	i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

	wndComboSvrName_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_SVRNAME));
	OnCbnSelchangeComboSvrName();
	wndComboServerDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_SVRDRV));
	wndComboRefreshTime_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_REFESHTIME));
	wndComboClientDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel ,MASK_VDISK_CLIDRV));
	wndComboSsdDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel ,MASK_VDISK_SSDDRV));

	stdex::tString size;
	stdex::ToString(val->Size, size);
	textSize_ = size.c_str();

	UpdateData(FALSE);
}

BOOL CDlgVDiskMgr::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndEditmsg_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wndListVDisks_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}

void CDlgVDiskMgr::_ShowView()
{
	size_t cnt = curvDisks_.size();
	wndListVDisks_.SetItemCount(cnt);
}

void CDlgVDiskMgr::OnLvnColumnclickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListVDisks_.GetItemCount() == 0 )
		return;


	CWaitCursor wc;
	std::stable_sort(curvDisks_.begin(), curvDisks_.end(), Compare(pNMLV->iSubItem, VDiskCompare[pNMLV->iSubItem]));
	VDiskCompare.set(pNMLV->iSubItem, !VDiskCompare[pNMLV->iSubItem]);
	wndListVDisks_.RedrawCurPageItems();

	wndListVDisks_.SetSortArrow(pNMLV->iSubItem, VDiskCompare[pNMLV->iSubItem]);
}


void CDlgVDiskMgr::OnLvnGetdispinfoListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curvDisks_.size() )
		return;
	using namespace i8desk::data_helper;
	const VDiskTraits::ValueType &val = curvDisks_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_VDISK_SVRNAME:
			text = i8desk::business::Server::GetServiceBySID(val->SvrID)->SvrName;
			break;
		case MASK_VDISK_IP:
			text = i8desk::IP2String(val->SoucIP);
			break;
		case MASK_VDISK_SVRDRV:
			text = val->SvrDrv;
			break;
		case MASK_VDISK_CLIDRV:
			text = val->CliDrv;
			break;
		case MASK_VDISK_REFESHTIME:
			text = i8desk::business::VDisk::GetLoadTypeDescByType(val->LoadType);
			break;
		case MASK_VDISK_SSDDRV:
			text = i8desk::business::VDisk::GetSsdDescByType(val->SsdDrv);
			break;
		case MASK_VDISK_SIZE:
			stdex::ToString(val->Size, text);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}


void CDlgVDiskMgr::OnCbnSelchangeComboSvrName()
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

	wndComboSvrIP_.SetCurSel(0);

}
