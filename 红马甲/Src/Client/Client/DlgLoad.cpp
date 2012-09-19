// DlgLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DlgLoad.h"
#include "afxdialogex.h"


#include "../../include/UI/ImageHelpers.h"
#include "../../include/Http/HttpHelper.h"
#include "../../include/Extend STL/StringAlgorithm.h"

#include "DataMgr.h"

// CDlgLoad 对话框
using namespace ui::frame;
IMPLEMENT_DYNAMIC(CDlgLoad, CNonFrameDlg)

CDlgLoad::CDlgLoad(CWnd* pParent /*=NULL*/)
	: CNonFrameDlg(this, CDlgLoad::IDD, pParent)
	, autoPsw_(false)
	, autoLoad_(false)
{

}

CDlgLoad::~CDlgLoad()
{
}

void CDlgLoad::DoDataExchange(CDataExchange* pDX)
{
	ui::frame::CNonFrameDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_LOAD_MIN, wndBtnMin_);
	DDX_Control(pDX, IDC_BUTTON_LOAD_CLOSE, wndBtnClose_);
	DDX_Control(pDX, IDC_STATIC_LOAD_USERNAME, wndLabelUserName_);
	DDX_Control(pDX, IDC_STATIC_LOAD_PSW, wndLabelPsw_);
	DDX_Control(pDX, IDC_EDIT_LOAD_USERNAME, wndEditUserName_);
	DDX_Control(pDX, IDC_EDIT_LOAD_PSW, wndEditPsw_);
	DDX_Control(pDX, IDC_CHECK_LOAD_REMEMBER_PSW, wndBtnRemember_);
	DDX_Control(pDX, IDC_CHECK_LOAD_AUTO, wndBtnAutoLoad_);
	DDX_Control(pDX, IDC_BUTTON_LOAD_LOAD, wndBtnLoad_);
	DDX_Control(pDX, IDC_BUTTON_LOAD_RESET, wndBtnReset_);
	DDX_Control(pDX, IDC_BUTTON_LOAD_UNKNOWN_PASSWORD, wndBtnUnknwonPsw_);
}


BEGIN_MESSAGE_MAP(CDlgLoad, ui::frame::CNonFrameDlg)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MIN, &CDlgLoad::OnBnClickedButtonLoadMin)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CLOSE, &CDlgLoad::OnBnClickedButtonLoadClose)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_LOAD_REMEMBER_PSW, &CDlgLoad::OnBnClickedCheckLoadRememberPsw)
	ON_BN_CLICKED(IDC_CHECK_LOAD_AUTO, &CDlgLoad::OnBnClickedCheckLoadAuto)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_LOAD, &CDlgLoad::OnBnClickedButtonLoadLoad)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_RESET, &CDlgLoad::OnBnClickedButtonLoadReset)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_UNKNOWN_PASSWORD, &CDlgLoad::OnBnClickedButtonLoadUnknownPassword)
END_MESSAGE_MAP()


// CDlgLoad 消息处理程序


BOOL CDlgLoad::OnInitDialog()
{
	ui::frame::CNonFrameDlg::OnInitDialog();

	bkImg_.Attach(ui::draw::AtlLoadGdiplusImage(IDB_PNG_LOAD_BK, _T("PNG")));

	HBITMAP min[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_MIN_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_MIN_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_MIN_CLICK, _T("PNG"))
	};
	wndBtnMin_.SetImages(min[0], min[1], min[2]);

	HBITMAP close[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CLOSE_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CLOSE_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CLOSE_CLICK, _T("PNG"))
	};
	wndBtnClose_.SetImages(close[0], close[1], close[2]);

	HBITMAP load[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_LOAD_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_LOAD_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_LOAD_CLICK, _T("PNG"))
	};
	wndBtnLoad_.SetImages(load[0], load[1], load[2]);

	HBITMAP reset[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_RESET_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_RESET_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_RESET_CLICK, _T("PNG"))
	};
	wndBtnReset_.SetImages(reset[0], reset[1], reset[2]);

	HBITMAP checkBox[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CHECKBOX_UNCHECK, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CHECKBOX_UNCHECK, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CHECKBOX_CHECK, _T("PNG"))
	};
	wndBtnAutoLoad_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndBtnRemember_.SetImages(checkBox[0], checkBox[1], checkBox[2]);


	HBITMAP unKnownPsw[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_UNKNOWNPSW_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_UNKNOWNPSW_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_UNKNOWNPSW_HOVER, _T("PNG"))
	};
	wndBtnUnknwonPsw_.SetImages(unKnownPsw[0], unKnownPsw[1], unKnownPsw[2]);

	SetChangeSize(false);
	SetWindowText(_T("红马甲用户服务终端"));

	ATL::CRegKey reg;
	if( reg.Open(HKEY_LOCAL_MACHINE, _T("Software\\红马甲"), KEY_QUERY_VALUE) == ERROR_SUCCESS )
	{
		DWORD autoLoad = 0;
		TCHAR psw[MAX_PATH] = {0};
		ULONG len = MAX_PATH;
		if( reg.QueryDWORDValue(_T("load"), autoLoad) == ERROR_SUCCESS &&
			autoLoad == 1 )
		{
			TCHAR userName[MAX_PATH] = {0};
			len = MAX_PATH;
			reg.QueryStringValue(_T("user"), userName, &len);

			len = MAX_PATH;
			reg.QueryStringValue(_T("psw"), psw, &len);


			wndEditUserName_.SetWindowText(userName);
			wndEditPsw_.SetWindowText(psw);

			autoLoad_ = true;
			autoPsw_ = true;

		}
		else if( reg.QueryStringValue(_T("psw"), psw, &len) == ERROR_SUCCESS &&
			psw[0] != '\0' )
		{
			TCHAR userName[MAX_PATH] = {0};
			len = MAX_PATH;
			reg.QueryStringValue(_T("user"), userName, &len);
			wndEditUserName_.SetWindowText(userName);
			wndEditPsw_.SetWindowText(psw);
			autoPsw_ = true;
		}
	}

	if( autoLoad_ )
		wndBtnRemember_.SetCheck(BST_CHECKED);

	if( autoPsw_ )
		wndBtnRemember_.SetCheck(BST_CHECKED);

	if( autoLoad_ )
		OnBnClickedButtonLoadLoad();

	return TRUE;  // return TRUE unless you set the focus to a control

}



void CDlgLoad::OnDestroy()
{
	ui::frame::CNonFrameDlg::OnDestroy();

}


BOOL CDlgLoad::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(*pDC, rcClient);

	bkImg_.Draw(memDC, rcClient);

	return TRUE;
}


void CDlgLoad::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	

}

void CDlgLoad::OnBnClickedButtonLoadMin()
{
	ShowWindow(SW_MINIMIZE);
}


void CDlgLoad::OnBnClickedButtonLoadClose()
{
	PostQuitMessage(0);
}



void CDlgLoad::OnBnClickedCheckLoadRememberPsw()
{
	autoPsw_ = wndBtnRemember_.GetCheck() == BST_CHECKED;
}


void CDlgLoad::OnBnClickedCheckLoadAuto()
{
	autoLoad_ = wndBtnAutoLoad_.GetCheck() == BST_CHECKED;
	if( autoLoad_ )
		wndBtnRemember_.SetCheck(BST_CHECKED);
}


void CDlgLoad::OnBnClickedButtonLoadLoad()
{
	UpdateData(FALSE);

	CString userName, psw;
	wndEditUserName_.GetWindowText(userName);
	wndEditPsw_.GetWindowText(psw);

	data::DataMgrInstance().SetUserInfo((LPCTSTR)userName, (LPCTSTR)psw);
	stdex::tString url = data::DataMgrInstance().GetUrl(0);

	struct OP
	{
		bool operator()(const char *, size_t) const
		{ return false; }
	};
	http::Buffer retBuf = http::DownloadFile2(url, OP());

	std::string bufTmp = retBuf.first.get();
	if( bufTmp[0] == 'N' )
	{
		AfxMessageBox(_T("密码错误,请重试"));
		return;
	}
	else
	{
		data::DataMgrInstance().SetPermission(stdex::ToNumber<size_t>(bufTmp));
		if( autoPsw_ )
		{
			ATL::CRegKey key;
			if( key.Create(HKEY_LOCAL_MACHINE, _T("Software\\红马甲")) == ERROR_SUCCESS )
			{
				key.SetStringValue(_T("psw"), psw);
				key.SetStringValue(_T("user"), userName);
			}
		}
		else
		{
			ATL::CRegKey key;
			if( key.Create(HKEY_LOCAL_MACHINE, _T("Software\\红马甲")) == ERROR_SUCCESS )
			{
				key.DeleteValue(_T("psw"));
				key.DeleteValue(_T("user"));
			}
		}

		if( autoLoad_ )
		{
			ATL::CRegKey key;
			if( key.Create(HKEY_LOCAL_MACHINE, _T("Software\\红马甲")) == ERROR_SUCCESS )
			{
				key.SetDWORDValue(_T("load"), 1);

				key.SetStringValue(_T("user"), userName);
				key.SetStringValue(_T("psw"), psw);
			}
		}
		else
		{
			ATL::CRegKey key;
			if( key.Create(HKEY_LOCAL_MACHINE, _T("Software\\红马甲")) == ERROR_SUCCESS )
			{
				key.DeleteValue(_T("load"));

				if( !autoPsw_ )
				{
					key.DeleteValue(_T("psw"));
					key.DeleteValue(_T("user"));
				}
			}
		}

		OnOK();
	}
}


void CDlgLoad::OnBnClickedButtonLoadReset()
{
	wndEditUserName_.SetWindowTextW(_T(""));
	wndEditPsw_.SetWindowTextW(_T(""));

	wndEditUserName_.SetFocus();
}


void CDlgLoad::OnBnClickedButtonLoadUnknownPassword()
{
	AfxMessageBox(_T("请致电红马甲VIP 服务热线：400-699-0897"));
}
