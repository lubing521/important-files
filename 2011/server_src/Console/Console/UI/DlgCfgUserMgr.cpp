#include "stdafx.h"
#include "../Console.h"
#include "DlgCfgUserMgr.h"
#include "DlgCfgUserSet.h"

#include "../ManagerInstance.h"
#include "../MessageBox.h"

#include "UIHelper.h"
#include "../DlgCheckUser.h"
#include "../Business/ConfigBusiness.h"
#include "../../../../include/ui/ImageHelpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgOptUserMgr 对话框

IMPLEMENT_DYNAMIC(CDlgCfgUserMgr, CDialog)

CDlgCfgUserMgr::CDlgCfgUserMgr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfgUserMgr::IDD, pParent)
	, consolepwd_(_T(""))
	, dateend_(_T(""))
	, bindid_(_T(""))
	, netname_(_T(""))
	, netid_(_T(""))
	, addr_(_T(""))
	, address_(_T(""))
	, telephone_(_T(""))
	, mobile_(_T(""))
	, bindname_(_T(""))
	, disktype_(_T(""))
	, netstatus_(_T(""))
	, feesname_(_T(""))
	, disklessname_(_T(""))
	, monitorname_(_T(""))
	, textNetbarJpg_(_T(""))

{
}

CDlgCfgUserMgr::~CDlgCfgUserMgr()
{
}

void CDlgCfgUserMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CFGUSERMGR_CONLOGINPWD, wndEditConsolePwd_);
	DDX_Text(pDX, IDC_EDIT_CFGUSERMGR_CONLOGINPWD, consolepwd_);

	DDX_Text(pDX, IDC_CFGUSERMGR_NETID,		netid_);
	DDX_Text(pDX, IDC_CFGUSERMGR_DATEEND,	dateend_);
	DDX_Text(pDX, IDC_CFGUSERMGR_BINDNAME,	bindname_ );
	DDX_Text(pDX, IDC_CFGUSERMGR_BINDID,	bindid_);
	DDX_Text(pDX, IDC_CFGUSERMGR_NETNAME,	netname_);
	DDX_Text(pDX, IDC_CFGUSERMGR_ADDR,		addr_);
	DDX_Text(pDX, IDC_CFGUSERMGR_ADDRESS,	address_);
	DDX_Text(pDX, IDC_CFGUSERMGR_TEL,		telephone_);
	//DDX_Text(pDX, IDC_CFGUSERMGR_PHONE,		mobile_);
	DDX_Text(pDX, IDC_CFGUSERMGR_DISKTYPE,	disktype_);
	DDX_Text(pDX, IDC_CFGUSERMGR_NETSTATUS,	netstatus_);
	DDX_Text(pDX, IDC_CFGUSERMGR_FEESNAME,	feesname_);
	DDX_Text(pDX, IDC_CFGUSERMGR_DISKLESSNAME,	disklessname_);
	DDX_Text(pDX, IDC_CFGUSERMGR_MONITORNAME,monitorname_);


	DDX_Control(pDX, IDC_CFGUSERMGR_NETID,		wndnetid_);
	DDX_Control(pDX, IDC_CFGUSERMGR_DATEEND,	wnddateend_);
	DDX_Control(pDX, IDC_CFGUSERMGR_BINDNAME,	wndbindname_ );
	DDX_Control(pDX, IDC_CFGUSERMGR_BINDID,		wndbindid_);
	DDX_Control(pDX, IDC_CFGUSERMGR_NETNAME,	wndnetname_);
	DDX_Control(pDX, IDC_CFGUSERMGR_ADDR,		wndaddr_);
	DDX_Control(pDX, IDC_CFGUSERMGR_ADDRESS,	wndaddress_);
	DDX_Control(pDX, IDC_CFGUSERMGR_TEL,		wndtelephone_);
	//DDX_Control(pDX, IDC_CFGUSERMGR_PHONE,		wndmobile_);
	DDX_Control(pDX, IDC_CFGUSERMGR_DISKTYPE,	wnddisktype_);
	DDX_Control(pDX, IDC_CFGUSERMGR_NETSTATUS,	wndnetstatus_);

	DDX_Control(pDX, IDC_CFGUSERMGR_FEESNAME,	wndfeesname_);
	DDX_Control(pDX, IDC_CFGUSERMGR_DISKLESSNAME,	wnddisklessname_);
	DDX_Control(pDX, IDC_CFGUSERMGR_MONITORNAME,wndmonitorname_);

	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_AUTHORIZE, wndAuthorize_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_NETID, wndNetID_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_DATEEND, wndDateEnd_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_BINDNAME, wndBindName_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_BINDID, wndBindID_);

	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_REG, wndReg_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_NETNAME, wndNetName_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_ADDR, wndAddr_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_ADDRESS, wndAddress_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_TEL, wndTel_);
	//DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_PHONE, wndPhone_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_DISKTYPE,	wndDisktype_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_NETSTATUS,	wndNetstatus_);

	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_FEESNAME,	wndFeesname_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_DISKLESSNAME,	wndDisklessname_);

	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_MONITORNAME,wndMonitorname_);

	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_CONPWD, wndConPwd_);
	DDX_Control(pDX, IDC_STATIC_CFGUSERMGR_CONLOGINPWD, wndConLoginPwd_);

	DDX_Control(pDX, IDC_BUTTON_CFGUSERMGR_MODIFY, wndBtnModify_);
	DDX_Control(pDX, IDC_BUTTON_CFGUSERMGR_MODIFYINFO, wndBtnModifyInfo_);


	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_NETBARJPG, wndLabelNetbarJpg_);
	DDX_Control(pDX, IDC_EDIT_CFGCLIENTSET_NETBARJPG, wndEditNetbarJpg_);
	DDX_Text(pDX, IDC_EDIT_CFGCLIENTSET_NETBARJPG, textNetbarJpg_);
	DDX_Control(pDX, IDC_BUTTON_CFGCLIENTSET_NETBARJPG, wndBtnNetbarJpg_);
	DDX_Control(pDX, IDC_STATIC_CFGCLIENTSET_NETBARJPGREVIEW, wndNetBarReview_);
	DDX_Control(pDX, IDC_STATIC_PIC_CFGCLIENTSET_NETBARJPG, wndNetBarPic_);


}

BEGIN_EASYSIZE_MAP(CDlgCfgUserMgr)   
	///*EASYSIZE(IDC_STATIC_CFGUSERMGR_AUTHORIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_REG, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_NETNAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_ADDR, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_ADDRESS, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_TEL, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_BINDNAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_CONPWD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_CONLOGINPWD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	//EASYSIZE(IDC_CFGUSERMGR_NETNAME, IDC_STATIC_CFGUSERMGR_NETNAME, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_ADDR, IDC_STATIC_CFGUSERMGR_ADDR, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_ADDRESS, IDC_STATIC_CFGUSERMGR_ADDRESS, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_TEL, IDC_STATIC_CFGUSERMGR_TEL, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_BINDNAME, IDC_STATIC_CFGUSERMGR_BINDNAME, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	//EASYSIZE(IDC_EDIT_CFGUSERMGR_CONLOGINPWD, IDC_STATIC_CFGUSERMGR_CONLOGINPWD, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	//EASYSIZE(IDC_STATIC_CFGUSERMGR_NETID, IDC_CFGUSERMGR_NETNAME, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_STATIC_CFGUSERMGR_PHONE, IDC_CFGUSERMGR_TEL, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//
	//EASYSIZE(IDC_CFGUSERMGR_DATEEND, IDC_STATIC_CFGUSERMGR_DATEEND, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_BINDID, IDC_STATIC_CFGUSERMGR_BINDID, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_NETID, IDC_STATIC_CFGUSERMGR_NETID, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	//EASYSIZE(IDC_CFGUSERMGR_PHONE, IDC_STATIC_CFGUSERMGR_PHONE, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)*/

END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgCfgUserMgr, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_MESSAGE(WM_MSG_OK, &CDlgCfgUserMgr::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON_CFGUSERMGR_MODIFY, &CDlgCfgUserMgr::OnBnClickedButtonCfgUserMgrModify)
	ON_BN_CLICKED(IDC_BUTTON_CFGUSERMGR_MODIFYINFO, &CDlgCfgUserMgr::OnBnClickedButtonCfgUserMgrModifyInfo)
	ON_BN_CLICKED(IDC_BUTTON_CFGUSERMGR_NETBARJPG, &CDlgCfgUserMgr::OnBnClickedNetbarJpg)

END_MESSAGE_MAP()


BOOL CDlgCfgUserMgr::OnInitDialog()
{
	CDialog::OnInitDialog();
//	INIT_EASYSIZE;

	// Edit
	wndEditConsolePwd_.LimitText(8);
	//wndNetBarPic_.ShowWindow(SW_HIDE);

	// 从系统选项表读取数据
	ReadData();

	outBox_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));


	return TRUE; 
}

void CDlgCfgUserMgr::ReadData()
{

	stdex::tString str ;
	
	//控制台登录密码
	consolepwd_ = i8desk::GetDataMgr().GetOptVal(OPT_U_CTLPWD,str).c_str();

	//授权期限
	dateend_	= i8desk::GetDataMgr().GetOptVal(OPT_U_DATEEND,str).c_str(); 
	dateend_	+= _T(" ") ;
	dateend_	+= i8desk::GetDataMgr().GetOptVal(OPT_U_USERTYPE,str).c_str();

	//上级ID
	bindid_		= i8desk::GetDataMgr().GetOptVal(OPT_U_OEMID,str).c_str();
	//网吧名称
	netname_	= i8desk::GetDataMgr().GetOptVal(OPT_U_NBNAME,str).c_str();
	//网吧ID
	netid_		= i8desk::GetDataMgr().GetOptVal(OPT_U_NID,str).c_str();
	//网吧所在省市
	/*addr_		= i8desk::GetDataMgr().GetOptVal(OPT_U_PROVINCE,str).c_str();
	addr_		+= _T(" ");*/
	addr_		= i8desk::GetDataMgr().GetOptVal(OPT_U_CITY,str).c_str();
	//网吧地址
	address_	= i8desk::GetDataMgr().GetOptVal(OPT_U_NBADDRESS,str).c_str();
	//网吧电话
	telephone_	= i8desk::GetDataMgr().GetOptVal(OPT_U_NBPHONE,str).c_str();
	//网吧手机
	mobile_		= i8desk::GetDataMgr().GetOptVal(OPT_U_NBMOBILE,str).c_str();
	//上级名称
	bindname_	= i8desk::GetDataMgr().GetOptVal(OPT_U_OEMNAME,str).c_str();
	// 磁盘类型
	int diskType = 0;
	diskType	= i8desk::GetDataMgr().GetOptVal(OPT_M_DISKTYPE, diskType);
	disktype_ = i8desk::business::config::GetDiskNameByType(diskType).c_str();
	// 网络状态
	int netStatus = 0;
	netStatus	= i8desk::GetDataMgr().GetOptVal(OPT_M_BROADBANDTYPE, netStatus);
	netstatus_ = i8desk::business::config::GetNetworkByType(netStatus).c_str();
	// 无盘名称
	disklessname_ = i8desk::GetDataMgr().GetOptVal(OPT_M_DISKPRODUCTNAME,str).c_str();
	// 计费软件名称
	feesname_	= i8desk::GetDataMgr().GetOptVal(OPT_M_FEEPRODUCTNAME,str).c_str();
	// 安全监控软件名称
	monitorname_= i8desk::GetDataMgr().GetOptVal(OPT_M_CULTURALNAME,str).c_str();

	//网吧图片路径
	str = _T("");
	textNetbarJpg_ = i8desk::GetDataMgr().GetOptVal(OPT_M_NETBARJPG, str).c_str();

	if(!textNetbarJpg_.IsEmpty())
		DrawPic((LPCTSTR)textNetbarJpg_, wndNetBarPic_);


	UpdateData(FALSE);
}



LRESULT CDlgCfgUserMgr::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	//确定按钮回写数据库选项表
	UpdateData(TRUE);
	i8desk::GetDataMgr().SetOptVal(OPT_U_CTLPWD, (LPCTSTR)consolepwd_);

	//网吧图片路径
	i8desk::GetDataMgr().SetOptVal(OPT_M_NETBARJPG, (LPCTSTR)textNetbarJpg_);


	return TRUE;
}

void CDlgCfgUserMgr::OnBnClickedNetbarJpg()
{

	UpdateData(TRUE);

	if( textNetbarJpg_.Right(1) == _T("\\") )
		textNetbarJpg_.Delete(textNetbarJpg_.GetLength() - 1);

	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	fileDlg.m_ofn.lpstrInitialDir = textNetbarJpg_;
	if( fileDlg.DoModal() == IDOK )
		fileName = fileDlg.GetPathName();
	else
		return;

	textNetbarJpg_ = fileName;

	if(!textNetbarJpg_.IsEmpty())
		DrawPic((LPCTSTR)textNetbarJpg_, wndNetBarPic_);


	UpdateData(FALSE);
}

void CDlgCfgUserMgr::DrawPic( LPCTSTR filename, i8desk::ui::SkinImage &wndpic)
{
	std::ifstream file(filename, std::ios_base::in |std::ios_base::binary);
	if( !file )
		return ;

	file.seekg(0, std::ios_base::end);
	std::streamoff len = file.tellg();
	file.seekg(0, std::ios_base::beg);

	assert(len != 0);
	std::vector<char> buf;
	buf.resize(static_cast<size_t>(len));

	file.read(&buf[0], len);


	HBITMAP bitmap = AtlLoadGdiplusImage(&buf[0], static_cast<size_t>(len));
	if(bitmap != NULL )
	{
		wndpic.SetImage(bitmap, false);
		wndpic.ShowWindow(SW_SHOW);
	}
}


BOOL CDlgCfgUserMgr::OnEraseBkgnd(CDC *pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(*pDC, rcClient);
	memDC.GetDC().FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		RGB(255, 255, 255));

	CRect rcWindow;
	wndEditConsolePwd_.GetWindowRect(rcWindow);
	i8desk::ui::DrawFrame(this, memDC, rcWindow, &outBox_);

	return TRUE;
}

void CDlgCfgUserMgr::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

}

void CDlgCfgUserMgr::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}

void CDlgCfgUserMgr::OnDestroy()
{
	CDialog::OnDestroy();
}

void CDlgCfgUserMgr::OnBnClickedButtonCfgUserMgrModify()
{
	CDlgCheckUser dlg;
	if( IDOK == dlg.DoModal() )
	{
		ReadData();

		CDlgCfgUserSet dlg;
		if( IDOK == dlg.DoModal() )
		{
			ReadData();
		}
	}
}

void CDlgCfgUserMgr::OnBnClickedButtonCfgUserMgrModifyInfo()
{
	CDlgCfgUserSet dlg;
	if( IDOK == dlg.DoModal() )
	{
		ReadData();
	}
}


