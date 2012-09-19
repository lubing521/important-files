// SysSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "SysSetDlg.h"
#include "CheckDiskDlg.h"
#include "MsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WXNDA_DEV	TEXT("\\Device\\wxNDA\\")

IMPLEMENT_DYNAMIC(CSysSetDlg, CDialog)

CSysSetDlg::CSysSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysSetDlg::IDD, pParent)
{
	
}

CSysSetDlg::~CSysSetDlg()
{
}

void CSysSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTION,		m_btnCaption);
	DDX_Control(pDX, IDC_CLOSE,			m_btnClose);
	DDX_Control(pDX, IDC_SVR_IP,		m_btnSvrIp);
	DDX_Control(pDX, IDC_EDIT_SVR_IP,	m_edtSvrIp);
	DDX_Control(pDX, IDC_AREA_SET,		m_btnAreaSet);
	DDX_Control(pDX, IDC_AREA_UNPROT,	m_btnUnProtArea);
	DDX_Control(pDX, IDC_AREA_PROT,		m_btnProtArea);
	DDX_Control(pDX, IDC_LEFT,			m_lstLeft);
	DDX_Control(pDX, IDC_RIGHT,			m_lstRigth);
	DDX_Control(pDX, IDC_ADD,			m_btnAdd);
	DDX_Control(pDX, IDC_ADDALL,		m_btnAddAll);
	DDX_Control(pDX, IDC_DEL,			m_btnDel);
	DDX_Control(pDX, IDC_DELALL,		m_btnDelAll);
	DDX_Control(pDX, IDC_BTN_CHECKDISK, m_btnCheckDisk);
	DDX_Control(pDX, IDOK,				m_btnOk);
	DDX_Control(pDX, IDCANCEL,			m_btnCancel);
}


BEGIN_MESSAGE_MAP(CSysSetDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CHECKDISK, &CSysSetDlg::OnBnClickedCheckDisk)
	ON_BN_CLICKED(IDOK,			&CSysSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,		&CSysSetDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ADD,		&CSysSetDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_ADDALL,	&CSysSetDlg::OnBnClickedAddall)
	ON_BN_CLICKED(IDC_DEL,		&CSysSetDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_DELALL,	&CSysSetDlg::OnBnClickedDelall)
	ON_BN_CLICKED(IDC_CLOSE, &CSysSetDlg::OnBnClickedClose)
END_MESSAGE_MAP()

BOOL CSysSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edtSvrIp.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	m_edtSvrIp.ModifyStyle(WS_BORDER, SWP_FRAMECHANGED);

	CHyOperator hy;
	bool bCanOpHy = hy.IsInitialize() && hy.IsInstallHy();

	CRect rcClient(0, 0, 514, 521), rc;
	CRgn rgn;
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);

	m_btnCaption.SetTextStyle(DEFAULT_FONT_NAME, 16, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),  I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, TRUE);
	m_btnCaption.MoveWindow(&CRect(5, 8, 200, 32));

	m_btnClose.SetImage(TEXT("Skin/Wnd/SysSet/按钮_关闭_默认状态.png"), TEXT("Skin/Wnd/SysSet/按钮_关闭_鼠标经过.png"),
		TEXT("Skin/Wnd/SysSet/按钮_关闭_按下.png"));
	m_btnClose.MoveWindow(&CRect(rcClient.right - 56, 3, rcClient.right - 56 + 41, 3 +24));

	m_btnSvrIp.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, TRUE);
	rc = CRect(50, 168, 50+90, 168+24);
	m_btnSvrIp.MoveWindow(&rc);
	
	m_edtSvrIp.MoveWindow(&CRect(157, 172, 157 + 332, 170 + 16));
	m_edtSvrIp.SetWindowText(AfxGetDbMgr()->LoadIpAddr());

	rc.OffsetRect(0, 60); rc.right += 80;
	m_btnAreaSet.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, TRUE);
	m_btnAreaSet.MoveWindow(&rc);

	rc.OffsetRect(-25, 30);
	m_btnUnProtArea.MoveWindow(&rc);

	rc.OffsetRect(300, 0);
	m_btnProtArea.MoveWindow(&rc);

	rc.left = 212; rc.top = 295;
	rc.right = rc.left + 97;
	rc.bottom = rc.top + 25;
	m_btnAdd.SetImage(TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮.png"), 
		TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮_鼠标经过.png"));
	m_btnAdd.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
	m_btnAdd.MoveWindow(&rc);

	rc.OffsetRect(0, 30);
	m_btnAddAll.SetImage(TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮.png"), 
		TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮_鼠标经过.png"));
	m_btnAddAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
	m_btnAddAll.MoveWindow(&rc);

	rc.OffsetRect(0, 40);
	m_btnDel.SetImage(TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮.png"), 
		TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮_鼠标经过.png"));
	m_btnDel.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
	m_btnDel.MoveWindow(&rc);
	rc.OffsetRect(0, 30);

	m_btnDelAll.SetImage(TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮.png"), 
		TEXT("Skin/Wnd/SysSet/背景_系统设置_按钮_鼠标经过.png"));
	m_btnDelAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
	m_btnDelAll.MoveWindow(&rc);

	rc = CRect(32, 290, 32 + 155, 290 + 140);
	m_lstLeft.MoveWindow(&rc);
	
	rc.OffsetRect(300, 0);
	m_lstRigth.MoveWindow(&rc);

	rc = CRect(25, 471, 25+102, 471+30);
	m_btnOk.SetImage(TEXT("Skin/Wnd/SysSet/按钮_确定修改_默认状态.png"), TEXT("Skin/Wnd/SysSet/按钮_确定修改_鼠标经过.png"));
	m_btnOk.MoveWindow(&rc);

	rc.OffsetRect(120, 0);
	m_btnCancel.SetImage(TEXT("Skin/Wnd/SysSet/按钮_取消修改_默认状态.png"), 
		TEXT("Skin/Wnd/SysSet/按钮_取消修改_鼠标经过.png"));
	m_btnCancel.MoveWindow(&rc);
	
	rc.OffsetRect(245, 0);
	m_btnCheckDisk.SetImage(TEXT("Skin/Wnd/SysSet/按钮_磁盘检测修复_默认状态.png"), TEXT("Skin/Wnd/SysSet/按钮_磁盘检测修复_鼠标经过.png"));
	m_btnCheckDisk.MoveWindow(&rc);

	DWORD nSize = GetLogicalDrives();
	for (int idx=2; idx<32; idx++)
	{
		TCHAR drv = TEXT('A') + idx;
		CString szItem = TEXT("X:");
		szItem.SetAt(0, drv);

		TCHAR szDev[MAX_PATH] = {0};
		if (QueryDosDevice(szItem, szDev, MAX_PATH) && StrNCmpI(WXNDA_DEV, szDev, _tcsclen(WXNDA_DEV)) == 0)
			continue ;

		szItem += TEXT("\\");
		if ((nSize & (1<<idx)) && (GetDriveType(szItem) == DRIVE_FIXED))
		{
			if (!bCanOpHy || !hy.IsDriverProtected(drv))
			{
				m_lstLeft.AddString(szItem);
			}
			else
			{
				m_lstRigth.AddString(szItem);
				m_strProtDrv += drv;
			}			
		}
	}
	if (!bCanOpHy)
	{
		m_lstLeft.EnableWindow(FALSE);
		m_lstRigth.EnableWindow(FALSE);
		m_btnAdd.EnableWindow(FALSE);
		m_btnAddAll.EnableWindow(FALSE);
		m_btnDel.EnableWindow(FALSE);
		m_btnDelAll.EnableWindow(FALSE);

		m_btnAdd.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(127, 127, 127), RGB(127, 127, 127),
			RGB(127, 127, 127), RGB(127, 127, 127), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
		m_btnAddAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(127, 127, 127), RGB(127, 127, 127),
			RGB(127, 127, 127), RGB(127, 127, 127), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
		m_btnDel.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(127, 127, 127), RGB(127, 127, 127),
			RGB(127, 127, 127), RGB(127, 127, 127), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
		m_btnDelAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(127, 127, 127), RGB(127, 127, 127),
			RGB(127, 127, 127), RGB(127, 127, 127), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);
	}

	MoveWindow(&rcClient, FALSE);
	SetWindowRgn(rgn, TRUE);

	CenterWindow();

	return TRUE;
}

BOOL CSysSetDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSysSetDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Wnd/SysSet/背景_系统设置.png"), rc);

	return TRUE;
}

LRESULT CSysSetDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

HBRUSH CSysSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(250, 250, 250));
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkColor(RGB(250, 250, 250));
		return brush;
	}
	return hbr;
}

void CSysSetDlg::OnBnClickedOk()
{
	CWaitCursor wc;
	CHyOperator hy;

	//保存IP地址
	CString szIP;
	m_edtSvrIp.GetWindowText(szIP);
	AfxGetDbMgr()->SaveIpAddr(szIP);
	if (hy.IsInitialize())
	{
		hy.ProtFile(AfxGetDbMgr()->GetIniFile());
	}

	//操作还原
	CString strUnProtDrv, strProtDrv;
	for (int idx=0; idx<m_lstLeft.GetCount(); idx++)
	{
		CString str;
		m_lstLeft.GetText(idx, str);
		strUnProtDrv += str[0];
	}

	for (int idx=0; idx<m_lstRigth.GetCount(); idx++)
	{
		CString str;
		m_lstRigth.GetText(idx, str);
		strProtDrv += str[0];
	}

	if (hy.IsInitialize())
	{
		bool bReboot1, bReboot2 = false;
		int ret1 = 0, ret2 = 0;
		if (strProtDrv != m_strProtDrv)
		{
			if (!strProtDrv.IsEmpty())
				ret1 = hy.ProtArea(strProtDrv, bReboot1);
		
			if (!strUnProtDrv.IsEmpty())
			    ret2 = hy.UnProtArea(strUnProtDrv, bReboot2);
	
			if ((ret1 == 0 || ret2 == 0) && (bReboot1 || bReboot2))
			{
				AfxMessageBoxEx(TEXT("还原操作成功，需要重新启动计算机."));
				hy.ReStartComputer();
				Sleep(2000);
				PostQuitMessage(0);
			}
		}
	}

	OnOK();
}

void CSysSetDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CSysSetDlg::OnBnClickedAdd()
{
	for (int idx=m_lstLeft.GetCount()-1; idx>=0; idx--)
	{
		if (m_lstLeft.GetSel(idx) == 1)
		{
			CString str;
			m_lstLeft.GetText(idx, str);
			m_lstRigth.AddString(str);
			m_lstLeft.DeleteString(idx);
		}
	}
}

void CSysSetDlg::OnBnClickedAddall()
{
	for (int idx=m_lstLeft.GetCount()-1; idx>=0; idx--)
	{
		CString str;
		m_lstLeft.GetText(idx, str);
		m_lstRigth.AddString(str);
		m_lstLeft.DeleteString(idx);
	}
}

void CSysSetDlg::OnBnClickedDel()
{
	for (int idx=m_lstRigth.GetCount()-1; idx>=0; idx--)
	{
		if (m_lstRigth.GetSel(idx) == 1)
		{
			CString str;
			m_lstRigth.GetText(idx, str);
			m_lstLeft.AddString(str);
			m_lstRigth.DeleteString(idx);
		}
	}
}

void CSysSetDlg::OnBnClickedDelall()
{
	for (int idx=m_lstRigth.GetCount()-1; idx>=0; idx--)
	{
		CString str;
		m_lstRigth.GetText(idx, str);
		m_lstLeft.AddString(str);
		m_lstRigth.DeleteString(idx);
	}
}

void CSysSetDlg::OnBnClickedClose()
{
	OnCancel();
}

void CSysSetDlg::OnBnClickedCheckDisk()
{
	CCheckDiskDlg dlg;
	dlg.DoModal();
}