// DlgClientAdd.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgClientAdd.h"

#include "../Misc.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Win32/System/SystemHelper.hpp"

#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../MessageBox.h"
#include "../Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
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

	

	enum { MASK_PREFIX = 1, MASK_SUFFIX, MASK_AREA, MASK_STARTNUM, MASK_STOPNUM, MASK_NUMLEN, MASK_IP, 
		MASK_MARK, MASK_NETGATE, MASK_DNS, MASK_DNS2 };

	template < typename ListCtrlT >
	void InitListClients(ListCtrlT &listCtrl)
	{
		listCtrl.SetHeaderImage(

			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_PREFIX,		_T("名字前缀"),	LVCFMT_LEFT, 65,	MASK_PREFIX,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_SUFFIX,		_T("名字后缀"),	LVCFMT_LEFT, 65,	MASK_SUFFIX,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_AREA,		_T("所属区域"),	LVCFMT_LEFT, 65,	MASK_AREA,		GetTrait());
		listCtrl.InsertColumnTrait(MASK_STARTNUM,	_T("起始编号"),	LVCFMT_LEFT, 80,	MASK_STARTNUM,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_STOPNUM,	_T("机器总数"),	LVCFMT_LEFT, 65,	MASK_STOPNUM,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_NUMLEN,		_T("编号长度"),	LVCFMT_LEFT, 65,	MASK_NUMLEN,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_IP,			_T("起始IP地址"),	LVCFMT_LEFT, 100,	MASK_IP,		GetTrait());
		listCtrl.InsertColumnTrait(MASK_MARK,		_T("子网掩码"),	LVCFMT_LEFT, 100,	MASK_MARK,		GetTrait());
		listCtrl.InsertColumnTrait(MASK_NETGATE,	_T("网关"),			LVCFMT_LEFT, 100,	MASK_NETGATE,	GetTrait());
		listCtrl.InsertColumnTrait(MASK_DNS,		_T("首选DNS"),		LVCFMT_LEFT, 100,	MASK_DNS,		GetTrait());
		listCtrl.InsertColumnTrait(MASK_DNS2,		_T("备用DNS"),		LVCFMT_LEFT, 100,	MASK_DNS2,		GetTrait());

		listCtrl.SetItemHeight(24);
	}
}



// CDlgClientAdd 对话框

IMPLEMENT_DYNAMIC(CDlgClientAdd, CNonFrameChildDlg)

CDlgClientAdd::CDlgClientAdd(const AreaTraits::ElementType *area, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientAdd::IDD, pParent)
	, namePre_(_T(""))
	, nameSuffix_(_T(""))
	, startNum_(0)
	, stopNum_(0)
	, numLen_(0)
	, curArea_(area)
	, wndComboArea_(CSize(24,24))
{

}

CDlgClientAdd::~CDlgClientAdd()
{
}

void CDlgClientAdd::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLIENT_NAME_PRE, namePre_);
	DDX_Text(pDX, IDC_EDIT_CLIENT_NAME_SUFFIX, nameSuffix_);
	DDX_Text(pDX, IDC_EDIT_CLIENT_FROM, startNum_);
	DDX_Text(pDX, IDC_EDIT_CLIENT_MACHINE_NUM, stopNum_);
	DDX_Text(pDX, IDC_EDIT_CLIENT_ENCODE_SIZE, numLen_);

	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_PRE, wndEditnamePre_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_NAME_SUFFIX, wndEditnameSuffix_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_FROM, wndEditstartNum_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_MACHINE_NUM, wndEditstopNum_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_ENCODE_SIZE, wndEditnumLen_);

	DDX_Control(pDX, IDC_COMBO_CLIENT_AREA, wndComboArea_);
	DDX_Control(pDX, IDC_IP_CLIENT_FROM, wndIPStart_);
	DDX_Control(pDX, IDC_IP_CLIENT_TO, wndIPStop_);
	DDX_Control(pDX, IDC_IP_CLIENT_NETGATE, wndIPGate_);
	DDX_Control(pDX, IDC_IP_CLIENT_MARK, wndIPMask_);
	DDX_Control(pDX, IDC_IP_CLIENT_DNS, wndIPDNS_);
	DDX_Control(pDX, IDC_IP_CLIENT_DNS2, wndIPDNS2_);

	DDX_Control(pDX, IDC_STATIC_CLIENT_NAME_PRE, wndLabelnamePre_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_NAME_SUFFIX, wndLabelnameSuffix_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_IPFROM, wndLabelstartNum_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MACHINE_NUM, wndLabelstopNum_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_ENCODE_SIZE, wndLabelnumLen_);

	DDX_Control(pDX, IDC_STATIC_CLIENT_AREA, wndLabelComboArea_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_IPTO, wndLabelIPStop_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_NETGATE, wndLabelIPGate_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MARK, wndLabelIPMask_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_DNS, wndLabelIPDNS_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_DNS2, wndLabelIPDNS2_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_START_NUM, wndLabelStartNum_);

	DDX_Control(pDX, IDC_STATIC_CLIENT_NAME, wndLabelName_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_AREASET, wndLabelAreaSet_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_PARAMSET, wndLabelParamSet_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_NUMSET, wndLabelNumSet_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_GROUPLIST, wndLabelGroupList_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_TRIGER, wndLabelTriger_);

	DDX_Control(pDX, IDC_BUTTON_CLIENT_ADD_ADD, wndBtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_ADD_MODIFY, wndBtnMod_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_ADD_REMOVE, wndBtnRemove_);
	DDX_Control(pDX, IDOK, wndBtnOk_);


	DDX_Control(pDX, IDC_LIST_CLIENT_CLIGRP, wndListClients_);
}


BEGIN_MESSAGE_MAP(CDlgClientAdd, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_ADD_ADD, &CDlgClientAdd::OnBnClickedButtonClientAddCli)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_ADD_MODIFY, &CDlgClientAdd::OnBnClickedButtonClientModCli)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_ADD_REMOVE, &CDlgClientAdd::OnBnClickedButtonClientRemoveCli)
	ON_BN_CLICKED(IDOK, &CDlgClientAdd::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CLIENT_CLIGRP, &CDlgClientAdd::OnNMClickListClientCligrp)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_CLIGRP, &CDlgClientAdd::OnLvnGetdispinfoListClientCligrp)

END_MESSAGE_MAP()


bool CDlgClientAdd::_CheckDatas(const ClientInfoPtr &val)
{
	UpdateData(TRUE);
	utility::Strcpy(val->prefix , namePre_);
	utility::Strcpy(val->suffix , nameSuffix_);

	typedef i8desk::data_helper::AreaTraits::ElementType AreaType;
	AreaType *area =  reinterpret_cast<AreaType *>(wndComboArea_.GetItemData(wndComboArea_.GetCurSel()));
	utility::Strcpy(val->area, area->AID);

	val->startnum = startNum_;
	val->stopnum = stopNum_;
	val->numlen = numLen_;
	wndIPStart_.GetAddress(val->ip);
	wndIPGate_.GetAddress(val->netgate);
	wndIPMask_.GetAddress(val->mark);
	wndIPDNS_.GetAddress(val->dns);
	wndIPDNS2_.GetAddress(val->dns2);
	val->ip		 = ::ntohl(val->ip);
	val->netgate = ::ntohl(val->netgate);
	val->mark	 = ::ntohl(val->mark);
	val->dns	 = ::ntohl(val->dns);
	val->dns2	 = ::ntohl(val->dns2);

	stdex::tString szip = i8desk::IP2String(val->ip);
	stdex::tString sznetgate = i8desk::IP2String(val->netgate);
	stdex::tString szdns = i8desk::IP2String(val->dns);
	stdex::tString szdns2 = i8desk::IP2String(val->dns2);

	if( !i8desk::IsValidIP(szip.c_str()) || !i8desk::IsValidIP(sznetgate.c_str()) ||
		!i8desk::IsValidIP(szdns.c_str()) || !i8desk::IsValidIP(szdns2.c_str()) )
	{
		CMessageBox msgDlg(_T("提示"),_T("IP地址不正确!"));
		msgDlg.DoModal();
		return false;
	}

	unsigned long curstopIp = val->ip  + val->stopnum ;
	unsigned long curfirstIp = val->ip;

	stdex::tOstringstream os;
	os << (LPCTSTR)namePre_ << _T("%0") << numLen_ << _T("d") << (LPCTSTR)nameSuffix_;

	stdex::tString curName = os.str();
	CString curFirstName;
	curFirstName.Format(curName.c_str(), startNum_);
	CString curLastName;
	curLastName.Format(curName.c_str(), startNum_ + stopNum_ - 1);

	for (size_t i = 0; i < clientInfos_.size(); i++) 
	{
		ClientInfoPtr tmp = clientInfos_[i];
		//检查机器名
		CString NameFormat, firstName, lastName;

		NameFormat.Format(_T("%s%%0%dd%s"), tmp->prefix ,tmp->numlen, tmp->suffix);

		firstName.Format(NameFormat, tmp->startnum);
		lastName.Format(NameFormat, tmp->startnum + tmp->stopnum - 1);

		if( (curFirstName >= firstName && curFirstName <= lastName)
			|| (curLastName >= firstName && curLastName <= lastName) ) 
		{
			stdex::tString msg = _T("设置的客户机名字与分组列表中存在重复\r\n");
			msg += _T("请检查命名规范与编号设置后再试!\n");
			CMessageBox msgDlg(_T("添加客户机"), msg);
			msgDlg.DoModal();

			return false;
		}

		// 检查IP地址
		CString firstIp, lastIp;

		unsigned long stopIP = tmp->ip + tmp->stopnum;
		if( (curfirstIp>= tmp->ip && curfirstIp <= stopIP)
			|| (curstopIp >= tmp->ip && curstopIp <= stopIP) ) 
		{
			stdex::tString msg = _T("设置的客户机名字与分组列表中存在重复\r\n");
			msg += _T("请检查起始IP地址与机器数量设置后再试!\n");
			CMessageBox msgDlg(_T("添加客户机"), msg);
			msgDlg.DoModal();
			return false;
		}
	}

	// 是否与已有的客户机冲突
	using i8desk::data_helper::ClientTraits;
	const ClientTraits::MapType &clients = i8desk::GetDataMgr().GetClients();

	for(ClientTraits::MapConstIterator iter = clients.begin(); iter != clients.end(); ++iter) 
	{
		if( curFirstName <= iter->second->Name && curLastName >= iter->second->Name) 
		{
			stdex::tString msg = _T("设置的客户机名字与分组列表中存在重复\r\n");
			msg += _T("请检查命名规范与编号设置后再试!\n");
			CMessageBox msgDlg(_T("添加客户机"), msg);
			msgDlg.DoModal();
			return false;
		}


		if( iter->second->IP >= curfirstIp && iter->second->IP <= curstopIp ) 
		{
			stdex::tString msg = _T("设置的客户机IP地址与已有的客户机存在重复\r\n");
			msg += _T("\r\n");
			msg += _T("请检查起始IP地址与机器数量设置后再试!\n");
			CMessageBox msgDlg(_T("添加客户机"), msg);
			msgDlg.DoModal();

			return false;
		}

	}

	UpdateData(FALSE);

	return true;
}


void CDlgClientAdd::_ShowView()
{
	size_t cnt = clientInfos_.size();
	wndListClients_.SetItemCount(cnt);
	if(cnt != 0 )
		wndBtnOk_.EnableWindow(TRUE);
	else
		wndBtnOk_.EnableWindow(FALSE);
}
// CDlgClientAdd 消息处理程序

BOOL CDlgClientAdd::OnInitDialog()
{

	std::vector<stdex::tString> dns;
	win32::system::GetLocalDNS(dns);

	std::vector<stdex::tString> gates;
	win32::system::GetLocalGateIps(gates);


	// 设置ComboBox皮肤
	HBITMAP arrow[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png"))

	};
	wndComboArea_.SetArrow(arrow[0], arrow[1], arrow[2]);


	HBITMAP bk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png"));
	HBITMAP editBk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboEditBK.png"));
	wndComboArea_.SetBk(bk, editBk);

	CNonFrameChildDlg::OnInitDialog();

	SetTitle(_T("添加客户机"));


	namePre_	= _T("A-");
	startNum_	= 1;
	stopNum_	= 1;
	numLen_		= 3;

	wndIPStart_.SetAddress(i8desk::String2IP(_T("192.168.1.1")));
	wndIPStop_.SetAddress(i8desk::String2IP(_T("192.168.1.1")));
	wndIPGate_.SetAddress(i8desk::String2IP(gates[0]));
	wndIPMask_.SetAddress(i8desk::String2IP(_T("255.255.255.0")));
	wndIPDNS_.SetAddress(i8desk::String2IP(dns[0]));
	wndIPDNS2_.SetAddress(i8desk::String2IP(dns[1]));


	IP_ADAPTER_INFO info[16] = {0};
	DWORD dwSize = sizeof(info);
	if( ERROR_SUCCESS != ::GetAdaptersInfo(info, &dwSize) )
	{
		PIP_ADAPTER_INFO pAdapter = info;
		if( pAdapter ) 
		{
			PIP_ADDR_STRING pIpAddress = &pAdapter->IpAddressList;
			if( pIpAddress ) 
				wndIPMask_.SetAddress(i8desk::String2IP(pIpAddress->IpMask.String));

			wndIPGate_.SetAddress(i8desk::String2IP(pAdapter->GatewayList.IpAddress.String));
			wndIPDNS_.SetAddress(i8desk::String2IP(pAdapter->SecondaryWinsServer.IpAddress.String));
			wndIPDNS2_.SetAddress(i8desk::String2IP(pAdapter->PrimaryWinsServer.IpAddress.String));

			// 缺省的客户机起始IP = SUBMASK & GATEWAY
			DWORD dwGatewayIp, dwMarkIp, dwFromIp;
			wndIPGate_.GetAddress(dwGatewayIp);
			wndIPMask_.GetAddress(dwMarkIp);
			dwFromIp = dwGatewayIp & dwMarkIp;
			wndIPStart_.SetAddress(dwFromIp);
		}
	}


	using i8desk::data_helper::AreaTraits;
	const AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		int nIndex = wndComboArea_.AddString(iter->second->Name);
		wndComboArea_.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(iter->second.get()));
	}

	if( curArea_ == 0 )
		wndComboArea_.SetCurSel(0);
	else
		wndComboArea_.SelectString(-1, curArea_->Name);

	// 初始化客户分组控件
	InitListClients(wndListClients_);

	wndBtnMod_.EnableWindow( FALSE );
	wndBtnRemove_.EnableWindow( FALSE );
	wndBtnOk_.EnableWindow( FALSE );

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnAdd_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnMod_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnRemove_.SetImages(btn[0],btn[1],btn[2]);


	// Edit

	wndEditnamePre_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditnameSuffix_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditstartNum_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditstopNum_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditnumLen_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	HBITMAP edit[] =
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
	};
	wndEditnamePre_.SetImages(edit[0], edit[1], edit[2]);
	wndEditnameSuffix_.SetImages(edit[0], edit[1], edit[2]);
	wndEditstartNum_.SetImages(edit[0], edit[1], edit[2]);
	wndEditstopNum_.SetImages(edit[0], edit[1], edit[2]);
	wndEditnumLen_.SetImages(edit[0], edit[1], edit[2]);


	// 画Static的背景
	wndLabelnamePre_.SetThemeParent(GetSafeHwnd());
	wndLabelnameSuffix_.SetThemeParent(GetSafeHwnd());
	wndLabelstartNum_.SetThemeParent(GetSafeHwnd());
	wndLabelstopNum_.SetThemeParent(GetSafeHwnd());
	wndLabelnumLen_.SetThemeParent(GetSafeHwnd());
	wndLabelComboArea_.SetThemeParent(GetSafeHwnd());
	wndLabelIPStart_.SetThemeParent(GetSafeHwnd());
	wndLabelIPStop_.SetThemeParent(GetSafeHwnd());
	wndLabelIPGate_.SetThemeParent(GetSafeHwnd());
	wndLabelIPMask_.SetThemeParent(GetSafeHwnd());
	wndLabelIPDNS_.SetThemeParent(GetSafeHwnd());
	wndLabelIPDNS2_.SetThemeParent(GetSafeHwnd());
	wndLabelName_.SetThemeParent(GetSafeHwnd());
	wndLabelAreaSet_.SetThemeParent(GetSafeHwnd());
	wndLabelParamSet_.SetThemeParent(GetSafeHwnd());
	wndLabelNumSet_.SetThemeParent(GetSafeHwnd());
	wndLabelGroupList_.SetThemeParent(GetSafeHwnd());
	wndLabelTriger_.SetThemeParent(GetSafeHwnd());
	wndLabelStartNum_.SetThemeParent(GetSafeHwnd());

	// IPAddress

	wndIPStart_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPStop_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPGate_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPMask_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPDNS_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPDNS2_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));


	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgClientAdd::OnBnClickedButtonClientAddCli()
{

	ClientInfoPtr val(new ClientInfo);
	if( !_CheckDatas(val) )
		return;

	clientInfos_.push_back(val);	
	_ShowView();

}

void CDlgClientAdd::OnBnClickedButtonClientModCli()
{
	int nSel = wndListClients_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;

	ClientInfoPtr val = clientInfos_[nSel];
	if( !_CheckDatas(val) )
		return;

	_ShowView();

}

void CDlgClientAdd::OnBnClickedButtonClientRemoveCli()
{
	int nSel = wndListClients_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;

	clientInfos_.erase(clientInfos_.begin() + nSel);
	_ShowView();

}

void CDlgClientAdd::OnBnClickedOk()
{
	using i8desk::data_helper::ClientTraits;
	ClientTraits::MapType &clients = i8desk::GetDataMgr().GetClients();

	for(size_t i = 0; i < clientInfos_.size(); i++)
	{
		ClientInfoPtr val = clientInfos_[i];
		size_t ipindex = 0;

		for(int n = val->startnum; n <  val->startnum + val->stopnum; n++) 
		{
			ClientTraits::ValueType client(new ClientTraits::ElementType);
			
			// 生成机器名
			stdex::tOstringstream os;
			os << (LPCTSTR)namePre_ << _T("%0") << numLen_ << _T("d") << (LPCTSTR)nameSuffix_;

			stdex::tString curName = os.str();
			CString MachineName;
			MachineName.Format(curName.c_str(), n);

			utility::Strcpy(client->Name, MachineName);

			// 生成IP地址
			client->IP		= ::ntohl(::htonl(val->ip) + ipindex );
			client->Gate	= val->netgate;
			client->Mark	= val->mark;
			client->DNS		= val->dns;
			client->DNS2	= val->dns2;

			utility::Strcpy(client->AID,val->area);
			ipindex++;

			// 发送命令
			if( !i8desk::GetDataMgr().AddData(clients, client->Name, client) ) 
			{
				stdex::tString msg = _T("添加客户机[name=");
				msg += MachineName;
				msg +=_T("]时失败"); 
				CMessageBox msgDlg(_T("添加客户机"), msg);
				msgDlg.DoModal();

				return;
			}
		}
	}

	OnOK();
}


void CDlgClientAdd::OnNMClickListClientCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	POSITION pos = wndListClients_.GetFirstSelectedItemPosition();
	bool b = pos != NULL;
	wndBtnMod_.EnableWindow(b);
	wndBtnRemove_.EnableWindow(b);

}

BOOL CDlgClientAdd::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListClients_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}



void CDlgClientAdd::OnLvnGetdispinfoListClientCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= clientInfos_.size() )
		return;
	using namespace i8desk::data_helper;
	const ClientInfoPtr val = clientInfos_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_PREFIX:
			 text = val->prefix;
			break;
		case MASK_SUFFIX:
			text = val->suffix;
			break;
		case MASK_AREA:
			{
				AreaTraits::MapIterator iter = i8desk::GetDataMgr().GetAreas().find(val->area);
				text = iter->second->Name;
			}
			break;
		case MASK_STARTNUM:
			stdex::ToString(val->startnum,text);
			break;
		case MASK_STOPNUM:
			stdex::ToString(val->stopnum,text);
			break;
		case MASK_NUMLEN:
			stdex::ToString(val->numlen,text);
			break;
		case MASK_IP:
			text = i8desk::IP2String(val->ip);
			break;
		case MASK_MARK:
			text = i8desk::IP2String(val->mark);
			break;
		case MASK_NETGATE:
			text = i8desk::IP2String(val->netgate);
			break;
		case MASK_DNS:
			text = i8desk::IP2String(val->dns);
			break;
		case MASK_DNS2:
			text = i8desk::IP2String(val->dns2);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}
