#include "StdAfx.h"
#include "BarOnline.h"
#include "CheckDiskDlg.h"

#define WXNDA_DEV	TEXT("\\Device\\wxNDA\\")

IMPLEMENT_DYNAMIC(CCheckDiskDlg, CDialog)

CCheckDiskDlg::CCheckDiskDlg(CWnd *pParent/*=NULL*/)
:CDialog(CCheckDiskDlg::IDD, pParent)
{

}

CCheckDiskDlg::~CCheckDiskDlg()
{

}

void CCheckDiskDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_TITLE,       m_btnTitle);
	DDX_Control(pDX, IDC_BTN_DRVLIST,     m_btnDrvList);
	DDX_Control(pDX, IDC_BTN_CHECKDRVLIST, m_btnCheckDrvList);
	DDX_Control(pDX, IDC_LST_LEFT,        m_lstLeft);
	DDX_Control(pDX, IDC_LST_RIGHT,       m_lstRigth);
	DDX_Control(pDX, IDC_BTN_ADD,         m_btnAdd);
	DDX_Control(pDX, IDC_BTN_ADDALL,      m_btnAddAll);
	DDX_Control(pDX, IDC_BTN_DEL,         m_btnDel);
	DDX_Control(pDX, IDC_BTN_DELALL,      m_btnDelAll);
	DDX_Control(pDX, IDC_BTN_CHECK,       m_btnCheck);
	DDX_Control(pDX, IDC_BTN_CANCEL,      m_btnCancel);
}

BEGIN_MESSAGE_MAP(CCheckDiskDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CANCEL,		&CCheckDiskDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CHECK,		&CCheckDiskDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_BTN_ADD,		    &CCheckDiskDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BTN_ADDALL,		&CCheckDiskDlg::OnBnClickedAddall)
	ON_BN_CLICKED(IDC_BTN_DEL,		    &CCheckDiskDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_BTN_DELALL,		&CCheckDiskDlg::OnBnClickedDelall)
END_MESSAGE_MAP()

BOOL CCheckDiskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rtClient(0, 0, 514, 315);

	CRgn rgn;
	rgn.CreateRoundRectRgn(rtClient.left, rtClient.top, rtClient.right, rtClient.bottom, 5, 5);

	m_btnTitle.SetTextStyle(DEFAULT_FONT_NAME, 16, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),  CButtonEx::BTEXT_LEFT, TRUE);
	m_btnTitle.MoveWindow(&CRect(40, 10, 160, 30));

	CRect rc(25, 60, 140, 75);
	m_btnDrvList.MoveWindow(&rc);

	rc.OffsetRect(300, 0);
	m_btnCheckDrvList.MoveWindow(&rc);

	rc.OffsetRect(-295, 18);
	rc.right  += 40;
	rc.bottom += 135;
	m_lstLeft.MoveWindow(&rc);

	rc.OffsetRect(300, 0);
	m_lstRigth.MoveWindow(&rc);

	rc.OffsetRect(-121, 10);
	rc.right  = rc.left + 97;
	rc.bottom = rc.top + 25;
	m_btnAdd.SetImage(TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮.png"), TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮_鼠标经过.png"));
	m_btnAdd.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER);
	m_btnAdd.MoveWindow(&rc);

	rc.OffsetRect(0, 30);
	m_btnAddAll.SetImage(TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮.png"), TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮_鼠标经过.png"));
	m_btnAddAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER);
	m_btnAddAll.MoveWindow(&rc);

	rc.OffsetRect(0, 40);
	m_btnDel.SetImage(TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮.png"), TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮_鼠标经过.png"));
	m_btnDel.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER);
	m_btnDel.MoveWindow(&rc);

	rc.OffsetRect(0, 30);
	m_btnDelAll.SetImage(TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮.png"), TEXT("Skin/Wnd/CheckDisk/背景_系统设置_按钮_鼠标经过.png"));
	m_btnDelAll.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), CButtonEx::BTEXT_CENTER);
	m_btnDelAll.MoveWindow(&rc);

	rc = CRect(125, 265, 227, 295);
	m_btnCheck.SetImage(TEXT("Skin/Wnd/CheckDisk/按钮_开始检测修复_默认状态.png"), TEXT("Skin/Wnd/CheckDisk/按钮_开始检测修复_鼠标经过.png"));
	m_btnCheck.MoveWindow(&rc);

	rc.OffsetRect(170, 0);
	m_btnCancel.SetImage(TEXT("Skin/Wnd/CheckDisk/按钮_取消_默认状态.png"), TEXT("Skin/Wnd/CheckDisk/按钮_取消_鼠标经过.png"));
	m_btnCancel.MoveWindow(&rc);

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
			TCHAR szContent[MAX_PATH + 6] = {0};
			lstrcat(szContent, szItem);
			GetVolumeInformation(szItem, szContent + 3, MAX_PATH + 1, NULL, NULL, NULL, NULL, 0);
			//把驱动器标识用（）括起来
			int nContentLen = lstrlen(szContent + 3);
			if (nContentLen > 0)
			{
				szContent[2] = '(';
				szContent[nContentLen + 3] = ')';
			}
			m_lstLeft.AddString(szContent);			
		}
	}

	MoveWindow(&rtClient, FALSE);
	SetWindowRgn(rgn, TRUE);

	CenterWindow();	
	return TRUE;
}

BOOL CCheckDiskDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CCheckDiskDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	AfxDrawImage(pDC, TEXT("Skin/Wnd/CheckDisk/背景_磁盘检测修复.png"), rc);

	return TRUE;
}

LRESULT CCheckDiskDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}


HBRUSH CCheckDiskDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CCheckDiskDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CCheckDiskDlg::OnBnClickedCheck()
{
	CString strCheckDrv;
	for (int idx=0; idx<m_lstRigth.GetCount(); idx++)
	{
		CString str;
		m_lstRigth.GetText(idx, str);
		strCheckDrv += str[0];
	}
	if (strCheckDrv.IsEmpty())
	{
		AfxMessageBoxEx(TEXT("请选择您要进行检测修复的磁盘！"));
		return;
	}

	TCHAR szFilePath[MAX_PATH] = {0}, szParam[MAX_PATH] = {0};
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	PathAddBackslash(szFilePath);
	lstrcat(szFilePath, TEXT("EncipherC.exe"));
	lstrcat(szParam, TEXT("/SetChkdsk "));
	lstrcat(szParam, strCheckDrv);

	ShellExecute(NULL, TEXT("open"), szFilePath, szParam, TEXT(""), SW_SHOWNORMAL);
}

void CCheckDiskDlg::OnBnClickedAdd()
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

void CCheckDiskDlg::OnBnClickedAddall()
{
	for (int idx=m_lstLeft.GetCount()-1; idx>=0; idx--)
	{
		CString str;
		m_lstLeft.GetText(idx, str);
		m_lstRigth.AddString(str);
		m_lstLeft.DeleteString(idx);
	}
}

void CCheckDiskDlg::OnBnClickedDel()
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

void CCheckDiskDlg::OnBnClickedDelall()
{
	for (int idx=m_lstRigth.GetCount()-1; idx>=0; idx--)
	{
		CString str;
		m_lstRigth.GetText(idx, str);
		m_lstLeft.AddString(str);
		m_lstRigth.DeleteString(idx);
	}
}