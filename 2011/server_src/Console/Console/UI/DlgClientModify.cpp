// DlgClientModify.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgClientModify.h"

#include "../../../../include/Utility/utility.h"
#include "../ui/Skin/SkinMgr.h"
#include "../Misc.h"
#include "../MessageBox.h"

// CDlgClientModify 对话框


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDlgClientModify, CNonFrameChildDlg)

CDlgClientModify::CDlgClientModify(const AreaTraits::ElementType *area, 
								   ClientTraits::VectorType &client, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientModify::IDD, pParent)
	, curArea_(area)
	, client_(client)
	, wndComboArea_(CSize(24, 24))
{

}

CDlgClientModify::~CDlgClientModify()
{
}

void CDlgClientModify::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP_CLIENT_MODIFY_START, wndIPStart_);
	DDX_Control(pDX, IDC_IP_CLIENT_MODIFY_GATE, wndIPGate_);
	DDX_Control(pDX, IDC_IP_CLIENT_MODIFY_DNS, wndIPDNS_);
	DDX_Control(pDX, IDC_CLIENT_MODIFY_MARK, wndIPMask_);
	DDX_Control(pDX, IDC_CLIENT_MODIFY_DNS2, wndIPDNS2_);
	DDX_Control(pDX, IDC_COMBO_CLIENT_MODIFY_AREA, wndComboArea_);

	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_START, wndLabelIPStart_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_GATE, wndLabelIPGate_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_DNS, wndLabelIPDNS_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_MARK, wndLabelIPMask_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_DNS2, wndLabelIPDNS2_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_MODIFY_AREA, wndLabelComboArea_);


	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

}


BEGIN_MESSAGE_MAP(CDlgClientModify, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgClientModify::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgClientModify::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgClientModify 消息处理程序

BOOL CDlgClientModify::OnInitDialog()
{

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
	SetTitle(_T("修改客户机属性"));	

	using i8desk::data_helper::AreaTraits;
	const AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		int nIndex = wndComboArea_.AddString(iter->second->Name);
		wndComboArea_.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(iter->second.get()));
	}
	if( curArea_ == 0 )
	{
		wndComboArea_.SetCurSel(0);
	}
	else
	{
		wndComboArea_.SelectString(-1, curArea_->Name);
	}

	wndIPStart_.SetAddress(::htonl(client_[0]->IP));
	wndIPGate_.SetAddress(::htonl(client_[0]->Gate));
	wndIPMask_.SetAddress(::htonl(client_[0]->Mark));
	wndIPDNS_.SetAddress(::htonl(client_[0]->DNS));
	wndIPDNS2_.SetAddress(::htonl(client_[0]->DNS2));

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);

	// 画Static的背景
	wndLabelIPStart_.SetThemeParent(GetSafeHwnd());
	wndLabelIPGate_.SetThemeParent(GetSafeHwnd());
	wndLabelIPDNS_.SetThemeParent(GetSafeHwnd());
	wndLabelIPMask_.SetThemeParent(GetSafeHwnd());
	wndLabelIPDNS2_.SetThemeParent(GetSafeHwnd());
	wndLabelComboArea_.SetThemeParent(GetSafeHwnd());

	// IPAddress

	wndIPStart_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPGate_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPMask_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPDNS_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndIPDNS2_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));


	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgClientModify::OnBnClickedOk()
{
	int nSel = wndComboArea_.GetCurSel();
	if (nSel == -1)
		return ;

	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndComboArea_.GetItemData(nSel));
	if( client_.size() == 1 )
	{
		if(!SetIP_(client_[0]))
			return;
		utility::Strcpy(client_[0]->AID, val->AID);
		if( !i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetClients(), client_[0]->Name, client_[0], 
			MASK_TCLIENT_AID | MASK_TCLIENT_IP | MASK_TCLIENT_GATE | MASK_TCLIENT_MARK | MASK_TCLIENT_DNS | MASK_TCLIENT_DNS2) )
			return;
	}
	else
	{
		for( size_t i = 0; i != client_.size(); ++i)
		{
			if(!SetIP_(client_[i]))
				return;
			utility::Strcpy(client_[i]->AID, val->AID);
			i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetClients(), client_[i]->Name, client_[i], 
				MASK_TCLIENT_AID | MASK_TCLIENT_GATE | MASK_TCLIENT_MARK | MASK_TCLIENT_DNS | MASK_TCLIENT_DNS2 );
		}
	}

	OnOK();
}

bool CDlgClientModify::SetIP_(ClientTraits::ValueType &client)
{
	DWORD nValue;
	wndIPStart_.GetAddress(nValue);
	client->IP = ::htonl(nValue);

	wndIPGate_.GetAddress(nValue);
	client->Gate = ::htonl(nValue);

	wndIPMask_.GetAddress(nValue);
	client->Mark = ::htonl(nValue);

	wndIPDNS_.GetAddress(nValue);
	client->DNS = ::htonl(nValue);

	wndIPDNS2_.GetAddress(nValue);
	client->DNS2 = ::htonl(nValue);

	stdex::tString szip = i8desk::IP2String(client->IP);
	stdex::tString sznetgate = i8desk::IP2String(client->Gate);
	stdex::tString szdns = i8desk::IP2String(client->DNS);
	stdex::tString szdns2 = i8desk::IP2String(client->DNS2);

	if( !i8desk::IsValidIP(szip.c_str()) || !i8desk::IsValidIP(sznetgate.c_str()) ||
		!i8desk::IsValidIP(szdns.c_str()) || !i8desk::IsValidIP(szdns2.c_str()) )
	{
		CMessageBox msgDlg(_T("提示"),_T("IP地址不正确!"));
		msgDlg.DoModal();
		return false;
	}

	return true;

}

void CDlgClientModify::OnBnClickedCancel()
{
	OnCancel();
}


