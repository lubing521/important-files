// DlgModify.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DlgModify.h"
#include "afxdialogex.h"

#include "../../include/UI/ImageHelpers.h"
#include "../../include/Http/HttpHelper.h"

#include "DataMgr.h"

// CDlgModify 对话框

using namespace ui::frame;

IMPLEMENT_DYNAMIC(CDlgModify, CNonFrameDlg)

CDlgModify::CDlgModify(CWnd* pParent /*=NULL*/)
	: CNonFrameDlg(this, CDlgModify::IDD, pParent)
{

}

CDlgModify::~CDlgModify()
{
}

void CDlgModify::DoDataExchange(CDataExchange* pDX)
{
	ui::frame::CNonFrameDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MODIFY_OLD_PSW, wndLabelOldPsw_);
	DDX_Control(pDX, IDC_STATIC_MODIFY_NEW_PSW, wndLabelNewPsw_);
	DDX_Control(pDX, IDC_STATIC_MODIFY_CONFIRM_PSW, wndLabelConfirmPsw_);
	DDX_Control(pDX, IDC_EDIT_OLD_PSW, wndEditOldPsw_);
	DDX_Control(pDX, ID_EDIT_NEW_PSW, wndEditNewPsw_);
	DDX_Control(pDX, IDC_EDIT_CONSUME_PSW, wndEditConfirmPsw_);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_OK, wndBtnOK_);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_CANCEL, wndBtnCancel_);
}


BEGIN_MESSAGE_MAP(CDlgModify, ui::frame::CNonFrameDlg)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_OK, &CDlgModify::OnBnClickedButtonModifyOk)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_CANCEL, &CDlgModify::OnBnClickedButtonModifyCancel)
END_MESSAGE_MAP()


// CDlgModify 消息处理程序


BOOL CDlgModify::OnInitDialog()
{
	ui::frame::CNonFrameDlg::OnInitDialog();
	bkImg_.Attach(ui::draw::AtlLoadGdiplusImage(IDB_PNG_MODIFY_BK, _T("PNG")));

	HBITMAP btnOKImg[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CONSUME_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CONSUME_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CONSUME_CLICK, _T("PNG"))
	};
	wndBtnOK_.SetImages(btnOKImg[0], btnOKImg[1], btnOKImg[2]);

	HBITMAP btnCancelImg[] = 
	{
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CANCEL_NORMAL, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CANCEL_HOVER, _T("PNG")),
		ui::draw::AtlLoadGdiplusImage(IDB_PNG_CANCEL_CLICK, _T("PNG"))
	};
	wndBtnCancel_.SetImages(btnCancelImg[0], btnCancelImg[1], btnCancelImg[2]);

	SetChangeSize(false);
	return TRUE;  // return TRUE unless you set the focus to a control
}



void CDlgModify::OnDestroy()
{
	ui::frame::CNonFrameDlg::OnDestroy();


}


void CDlgModify::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	

}


BOOL CDlgModify::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(*pDC, rcClient);

	bkImg_.Draw(memDC, rcClient);

	return TRUE;
}


void CDlgModify::OnBnClickedButtonModifyOk()
{
	CString oldPsw, newPsw, newPswTmp;
	wndEditOldPsw_.GetWindowText(oldPsw);
	wndEditNewPsw_.GetWindowText(newPsw);
	wndEditConfirmPsw_.GetWindowText(newPswTmp);

	if( (LPCTSTR)oldPsw != data::DataMgrInstance().GetUserInfo().second )
	{
		AfxMessageBox(_T("输入的旧密码错误,请重新输入"));
		return;
	}

	if( newPsw != newPswTmp )
	{
		AfxMessageBox(_T("两次输入的新密码不符,请重新输入"));
		return;
	}

	std::wstring url = data::DataMgrInstance().GetUrl(1);
	url += _T("&repassword=");
	url += (LPCTSTR)newPsw;

	struct OP
	{
		bool operator()(const char *, size_t) const
		{ return false; }
	};
	http::Buffer retBuf = http::DownloadFile2(url, OP());

	const char *bufTmp = retBuf.first.get();
	if( bufTmp[0] == 'Y' )
	{
		data::DataMgrInstance().SetUserInfo(data::DataMgrInstance().GetUserInfo().first, (LPCTSTR)newPsw);

		ATL::CRegKey key;
		if( key.Create(HKEY_LOCAL_MACHINE, _T("Software\\红马甲")) == ERROR_SUCCESS )
		{
			key.DeleteValue(_T("user"));
			key.DeleteValue(_T("load"));
			key.DeleteValue(_T("psw"));
		}

		AfxMessageBox(_T("修改密码成功, 请重新登录"));

		OnOK();
	}
	else
	{
		AfxMessageBox(_T("修改密码服务端出错!"));
		OnCancel();
	}
}


void CDlgModify::OnBnClickedButtonModifyCancel()
{
	OnCancel();
}
